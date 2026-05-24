#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreCameraMan.h>
#include <OgreRTShaderSystem.h>

#include <format>
#include <glm/glm.hpp>
#include <iostream>

#include "auxiliary.hpp"
#include "hair_auxiliary.hpp"
// #include "hair_roots_allocation.hpp"

class KeyHandler : public OgreBites::InputListener {
 public:
  bool keyPressed(const OgreBites::KeyboardEvent& evt) override {
    if (evt.keysym.sym == OgreBites::SDLK_ESCAPE) {
      Ogre::Root::getSingleton().queueEndRendering();
    }
    return true;
  }
};

// NOLINTBEGIN
int main() {
  OgreBites::ApplicationContext ctx("RenderHair");
  ctx.initApp();

  Ogre::Root* root = ctx.getRoot();
  Ogre::SceneManager* scnMgr = root->createSceneManager();

  // 1. Настройка RTSS (Критично для Ogre 14)
  Ogre::RTShader::ShaderGenerator* shadergen =
      Ogre::RTShader::ShaderGenerator::getSingletonPtr();
  shadergen->addSceneManager(scnMgr);

  // Указываем вьюпорту использовать схему шейдеров RTSS
  Ogre::Camera* cam = scnMgr->createCamera("myCam");
  Ogre::Viewport* viewport = ctx.getRenderWindow()->addViewport(cam);
  viewport->setMaterialScheme(
      Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
  viewport->setBackgroundColour(
      Ogre::ColourValue(0.1F, 0.1F, 0.1F));  // Серый фон для теста

  // 2. Свет (сделаем его поярче и подальше)
  scnMgr->setAmbientLight(Ogre::ColourValue(0.1F, 0.1F, 0.1F));
  Ogre::Light* light = scnMgr->createLight("MainLight");
  light->setType(Ogre::Light::LT_DIRECTIONAL);

  Ogre::SceneNode* lightNode =
      scnMgr->getRootSceneNode()->createChildSceneNode();
  lightNode->setPosition(20, 80, 50);
  lightNode->attachObject(light);

  // 3. Камера
  Ogre::SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
  camNode->setPosition(
      0, 0, 300);  // Отодвинем камеру подальше (sphere.mesh часто большая)
  camNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TS_WORLD);

  cam->setNearClipDistance(1.0);
  cam->setAutoAspectRatio(true);
  camNode->attachObject(cam);

  // 4. Объект
  Ogre::Entity* ent = scnMgr->createEntity("HairPlane.mesh");

  auto filename = ent->getMesh()->getName();
  auto hair_map_filename =
      RenderHair::Auxiliary::getHairDistributionMapFilename(filename, ".png");

  Ogre::Image hair_distr_map;

  try {
    hair_distr_map.load(
        hair_map_filename,
        Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
  } catch (const Ogre::Exception& error) {
    Ogre::LogManager::getSingleton().logMessage("Could not find density map: " +
                                                hair_map_filename);
  }

  auto sampler = [&hair_distr_map](const glm::vec2& uv) {
    const float u = std::clamp(uv.x, 0.0f, 1.0f);
    const float v = std::clamp(1.0f - uv.y, 0.0f, 1.0f);

    const auto x = static_cast<uint32_t>(
        u * static_cast<float>(hair_distr_map.getWidth() - 1));
    const auto y = static_cast<uint32_t>(
        v * static_cast<float>(hair_distr_map.getHeight() - 1));

    const Ogre::ColourValue pixel = hair_distr_map.getColourAt(x, y, 0);

    return glm::vec4(pixel.r, pixel.g, pixel.b, pixel.a);
  };

  auto mesh = RenderHair::App::convertMeshToTriangles(*ent->getMesh());
  int indx = 0;
  std::cout << "triangles count : " << mesh.size() << '\n';
  for (const auto& triangle : mesh) {
    std::cout << "triangle #" << indx++ << '\n';
    for (const auto& vertex : triangle.vertexes) {
      std::cout << std::format("\tPosition [{}, {}, {}]\n", vertex.x, vertex.y,
                               vertex.z);
    }
  }

  auto hair_roots =
      RenderHair::HairRootsAllocation::allocate(mesh, sampler, 1000.0F, 5.0F);

  indx = 0;
  std::cout << "hair roots count : " << hair_roots.size() << '\n';
  for (const auto& hair_root : hair_roots) {
    std::cout << std::format("Hair Root #{}", indx++) << '\n';
    std::cout << std::format("\tPosition : [{}; {}; {}]", hair_root.position.x,
                             hair_root.position.y, hair_root.position.y)
              << '\n';
    std::cout << std::format("\tDirection : [{}, {}, {}]",
                             hair_root.direction.x, hair_root.direction.y,
                             hair_root.direction.z);
    std::cout << std::format("\tLength : {}", hair_root.length) << '\n';
  }

  // RenderHair::HairRootsAllocation::allocate();

  // Попробуем принудительно обновить RTSS для этого объекта
  // ent->setMaterialName("Ogre/Eyes");

  Ogre::SceneNode* node = scnMgr->getRootSceneNode()->createChildSceneNode();
  node->attachObject(ent);

  node->setScale(Ogre::Vector3f(25, 25, 25));

  // 5. Управление
  OgreBites::CameraMan camMan(camNode);
  camMan.setStyle(OgreBites::CS_FREELOOK);
  ctx.addInputListener(&camMan);

  KeyHandler keyHandler;
  ctx.addInputListener(&keyHandler);

  // Чтобы мышь не убегала
  ctx.setWindowGrab(true);

  root->startRendering();
  ctx.closeApp();

  return 0;
}
// NOLINTEND
