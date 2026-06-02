#include <Ogre.h>
#include <OgreApplicationContext.h>
#include <OgreCameraMan.h>
#include <OgreRTShaderSystem.h>
#include <OgreTrays.h>

#include "render_hair/render/collision/collider_manager.hpp"
#include "render_hair/render/hair_instance.hpp"
#include "render_hair/render/verlet_cpu_hair_physics.hpp"
// #include "render_hair/render/verlet_gpu_hair_physics.hpp"

using HairPhysicsType = RenderHair::VerletCPU_HairPhysics;

class KeyHandler : public OgreBites::InputListener {
 private:
  Ogre::SceneNode* sphere_node_;
  Ogre::SceneNode* light_node_;
  float move_speed_ = 0.05F;  // Скорость движения сферы
  float rotation_speed_ = 0.01F;

 public:
  explicit KeyHandler(Ogre::SceneNode* sphere_node, Ogre::SceneNode* light_node)
      : sphere_node_(sphere_node),
        light_node_{light_node} {}

  bool keyPressed(const OgreBites::KeyboardEvent& evt) override {
    if (evt.keysym.sym == OgreBites::SDLK_ESCAPE) {
      Ogre::Root::getSingleton().queueEndRendering();
      return true;
    }

    if (!sphere_node_) return true;

    Ogre::Vector3 position = sphere_node_->getPosition();
    Ogre::Vector3 light_pos = light_node_->getPosition();
    float rotation = 0.0F;

    // IJKL управление по осям X и Z
    if (evt.keysym.sym == 'i') { position.z -= move_speed_; }
    if (evt.keysym.sym == 'k') { position.z += move_speed_; }
    if (evt.keysym.sym == 'j') { position.x -= move_speed_; }
    if (evt.keysym.sym == 'l') { position.x += move_speed_; }
    if (evt.keysym.sym == 'u') { sphere_node_->pitch(Ogre::Radian{rotation_speed_}); }
    if (evt.keysym.sym == 'o') { sphere_node_->pitch(Ogre::Radian{-rotation_speed_}); }

    if (evt.keysym.sym == 'h') { light_pos.z -= move_speed_; }
    if (evt.keysym.sym == 'n') { light_pos.z += move_speed_; }
    if (evt.keysym.sym == 'b') { light_pos.x -= move_speed_; }
    if (evt.keysym.sym == 'm') { light_pos.x += move_speed_; }

    if (evt.keysym.sym == ',') { light_pos.y -= move_speed_; }
    if (evt.keysym.sym == '.') { light_pos.y += move_speed_; }

    sphere_node_->setPosition(position);
    light_node_->setPosition(light_pos);
    return true;
  }
};

template <class HairType>
class HairFrameListener : public Ogre::FrameListener {
  HairType& hair_;
  RenderHair::Collider::SphereCollider* sphere_collider_;
  Ogre::SceneNode& node_;

 public:
  HairFrameListener(HairType& hair, RenderHair::Collider::SphereCollider* sphere_collider, Ogre::SceneNode& node)
      : hair_{hair},
        sphere_collider_{sphere_collider},
        node_{node} {}

  bool frameRenderingQueued(const Ogre::FrameEvent& evt) override {
    hair_.update(evt.timeSinceLastFrame);
    Ogre::Matrix4 world_mat = node_._getFullTransform();
    if (sphere_collider_ == nullptr) { return true; }
    sphere_collider_->update(world_mat);
    return true;
  }
};

// NOLINTBEGIN
int main() {
  OgreBites::ApplicationContext ctx("RenderHair");
  ctx.initApp();
  ctx.getRenderWindow()->setVSyncEnabled(true);
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
  light->setType(Ogre::Light::LT_POINT);
  light->setDiffuseColour(Ogre::ColourValue(1.0f, 1.0f, 0.9f));  // Основной свет (чуть желтоватый, как солнце)
  light->setSpecularColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));

  Ogre::SceneNode* light_node = scnMgr->getRootSceneNode()->createChildSceneNode();
  light_node->setPosition(2, 2, 2);
  light_node->attachObject(light);
  light_node->lookAt(Ogre::Vector3(0.0f, 0.0f, 0.0f).normalisedCopy(), Ogre::Node::TS_WORLD);

  // 3. Камера
  Ogre::SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();
  camNode->setPosition(0, 0, 20);
  camNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TS_WORLD);

  cam->setNearClipDistance(1.0);
  cam->setAutoAspectRatio(true);
  camNode->attachObject(cam);

  // 4. Управление
  OgreBites::CameraMan camMan(camNode);
  camMan.setTopSpeed(5);
  camMan.setStyle(OgreBites::CS_FREELOOK);
  ctx.addInputListener(&camMan);

  auto* overlay_system = ctx.getOverlaySystem();
  scnMgr->addRenderQueueListener(overlay_system);
  auto* tray_manager = new OgreBites::TrayManager("Interface", ctx.getRenderWindow());
  ctx.addInputListener(tray_manager);
  // Показываем статистику
  tray_manager->showFrameStats(OgreBites::TL_BOTTOMLEFT);

  // ~~~~~

  Ogre::Entity* ent = scnMgr->createEntity("Head.mesh");

  Ogre::SceneNode* node = scnMgr->getRootSceneNode()->createChildSceneNode();
  node->yaw(Ogre::Radian{3.1415F / 2});
  node->pitch(Ogre::Radian{3.1415F / 2});
  // node->setScale(50, 50, 50);
  node->attachObject(ent);

  RenderHair::Collider::Sphere sphere{Ogre::Vector3{-0.01F, 0.F, -0.65F}, 0.7F};
  RenderHair::Collider::SphereCollider collider =
      RenderHair::ColliderManager::getInstance().registerSphereCollider(sphere);

  RenderHair::HairInstance<HairPhysicsType>::Settings hair_settings = {.nodes_per_hair = 10,
                                                                       .one_hair_length = 0.3F,
                                                                       .scene_manager = scnMgr,
                                                                       .target_node = node,
                                                                       .mesh = ent->getMesh()};
  auto* hair_instance = new RenderHair::HairInstance<HairPhysicsType>(hair_settings);

  auto* hairListener =
      new HairFrameListener<RenderHair::HairInstance<HairPhysicsType>>(*hair_instance, &collider, *node);
  root->addFrameListener(hairListener);
  // ~~~~~

  KeyHandler keyHandler{node, light_node};
  ctx.addInputListener(&keyHandler);

  // Чтобы мышь не убегала
  ctx.setWindowGrab(true);

  root->startRendering();
  ctx.closeApp();

  return 0;
}
// NOLINTEND
