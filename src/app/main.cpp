#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreCameraMan.h>
#include <OgreRTShaderSystem.h>

#include "render_hair/render/hair_generator.hpp"
#include "render_hair/render/hair_model.hpp"
#include "render_hair/render/mesh_conversion.hpp"

class KeyHandler : public OgreBites::InputListener {
 public:
  bool keyPressed(const OgreBites::KeyboardEvent& evt) override {
    if (evt.keysym.sym == OgreBites::SDLK_ESCAPE) { Ogre::Root::getSingleton().queueEndRendering(); }
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
  Ogre::RTShader::ShaderGenerator* shadergen = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
  shadergen->addSceneManager(scnMgr);

  // Указываем вьюпорту использовать схему шейдеров RTSS
  Ogre::Camera* cam = scnMgr->createCamera("myCam");
  Ogre::Viewport* viewport = ctx.getRenderWindow()->addViewport(cam);
  viewport->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
  viewport->setBackgroundColour(Ogre::ColourValue(0.1F, 0.1F, 0.1F));

  // 2. Свет (сделаем его поярче и подальше)
  scnMgr->setAmbientLight(Ogre::ColourValue(0.1F, 0.1F, 0.1F));
  Ogre::Light* light = scnMgr->createLight("MainLight");
  light->setType(Ogre::Light::LT_DIRECTIONAL);
  light->setDiffuseColour(Ogre::ColourValue(1.0f, 1.0f, 0.9f));  // Основной свет (чуть желтоватый, как солнце)
  light->setSpecularColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));

  Ogre::SceneNode* lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
  lightNode->setPosition(20, 80, 50);
  lightNode->attachObject(light);
  lightNode->lookAt(Ogre::Vector3(-1.0f, -1.0f, -1.0f).normalisedCopy(), Ogre::Node::TS_WORLD);

  // 3. Камера
  Ogre::SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
  camNode->setPosition(0, 0, 300);
  camNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TS_WORLD);

  cam->setNearClipDistance(1.0);
  cam->setAutoAspectRatio(true);
  camNode->attachObject(cam);

  // 4. Управление
  OgreBites::CameraMan camMan(camNode);
  camMan.setStyle(OgreBites::CS_FREELOOK);
  ctx.addInputListener(&camMan);

  KeyHandler keyHandler;
  ctx.addInputListener(&keyHandler);

  // Чтобы мышь не убегала
  ctx.setWindowGrab(true);

  // ~~~~~

  Ogre::Entity* ent = scnMgr->createEntity("HairPlane.mesh");

  Ogre::SceneNode* node = scnMgr->getRootSceneNode()->createChildSceneNode();
  node->attachObject(ent);

  node->setScale(Ogre::Vector3f(50, 50, 50));

  auto triangles = RenderHair::Render::convert_mesh_to_triangles(ent->getMesh());
  auto roots = RenderHair::Render::generatePrimitiveRoots(triangles, 1);

  auto* hairModel = new RenderHair::Render::HairModel(*scnMgr, *node, ent->getMesh(), roots);

  // ~~~~~

  root->startRendering();
  ctx.closeApp();

  return 0;
}
// NOLINTEND
