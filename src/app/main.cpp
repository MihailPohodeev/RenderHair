#include <Ogre.h>
#include <OgreApplicationContext.h>

#include "connected_nodes.hpp"

using namespace RenderHair;

class HairSimulationApp : public OgreBites::ApplicationContext,
                          public OgreBites::InputListener {
  ConnectedNodes* hair;
  Ogre::ManualObject* manualObj;
  Ogre::SceneNode* hairNode;
  glm::vec3 gravity = {0.0f, -9.8f, 0.0f};

 public:
  HairSimulationApp() : OgreBites::ApplicationContext("OgreHairSim") {}

  void setup() override {
    OgreBites::ApplicationContext::setup();
    addInputListener(this);

    Ogre::Root* root = getRoot();
    Ogre::SceneManager* scnMgr = root->createSceneManager();

    // Свет и камера
    Ogre::SceneNode* camNode =
        scnMgr->getRootSceneNode()->createChildSceneNode();
    Ogre::Camera* cam = scnMgr->createCamera("MainCam");
    cam->setNearClipDistance(5);
    camNode->attachObject(cam);
    camNode->setPosition(0, 0, 20);
    getRenderWindow()->addViewport(cam);

    // Инициализация физики волос
    ConnectedNodes::Configuration config{10.0f, 2, 0.98f, 5};
    hair = new ConnectedNodes(config);

    // Создание объекта отрисовки
    manualObj = scnMgr->createManualObject("HairVisual");
    manualObj->setDynamic(true);
    hairNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    hairNode->attachObject(manualObj);
  }

  bool frameStarted(const Ogre::FrameEvent& evt) override {
    // 1. Обновление физики (добавляем гравитацию)
    hair->update(evt.timeSinceLastFrame, gravity);

    // 2. Отрисовка линии
    manualObj->clear();
    manualObj->begin("BaseWhiteNoLighting",
                     Ogre::RenderOperation::OT_LINE_STRIP);

    for (int i = 0; i < hair->getNodesCount(); ++i) {
      glm::vec3 p = hair->getNodePosition(i);
      manualObj->position(p.x, p.y, p.z);
    }

    manualObj->end();
    return true;
  }

  bool keyPressed(const OgreBites::KeyboardEvent& evt) override {
    if (evt.keysym.sym == OgreBites::SDLK_ESCAPE)
      getRoot()->queueEndRendering();
    return true;
  }
};

int main() {
  HairSimulationApp app;
  app.initApp();
  app.getRoot()->startRendering();
  app.closeApp();
  return 0;
}
