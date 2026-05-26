#pragma once
#include <OgreHardwareBufferManager.h>
#include <OgreSimpleRenderable.h>

namespace RenderHair::Render {

  class HairRenderable : public Ogre::SimpleRenderable {
   public:
    HairRenderable(Ogre::HardwareVertexBufferSharedPtr particleBuffer);
    ~HairRenderable() override;

    // Важнейшие переопределения для Ogre
    Ogre::Real getBoundingRadius(void) const override { return 1000.0f; }  // Чтобы не отсекался камерой
    Ogre::Real getSquaredViewDepth(const Ogre::Camera*) const override { return 0.0f; }

    // Метод, вызываемый Ogre непосредственно перед отправкой на рендер
    void _updateRenderQueue(Ogre::RenderQueue* queue) override;

   private:
    Ogre::HardwareVertexBufferSharedPtr mParticleBuffer;
  };

}  // namespace RenderHair::Render
