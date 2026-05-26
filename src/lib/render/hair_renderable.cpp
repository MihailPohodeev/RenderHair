#include "render_hair/render/hair_renderable.hpp"

#include <OgreHardwareBufferManager.h>
#include <OgreRenderQueue.h>

namespace RenderHair::Render {

  HairRenderable::HairRenderable(Ogre::HardwareVertexBufferSharedPtr vertexBuffer) {
    mRenderOp.vertexData = new Ogre::VertexData();
    mRenderOp.vertexData->vertexCount = 10;  // Ровно 10 точек
    mRenderOp.useIndexes = false;
    mRenderOp.operationType = Ogre::RenderOperation::OT_POINT_LIST;  // Рисуем точки

    // Объявляем, что в буфере лежат обычные 3D координаты (VES_POSITION)
    Ogre::VertexDeclaration* decl = mRenderOp.vertexData->vertexDeclaration;
    decl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);

    // Привязываем буфер
    mRenderOp.vertexData->vertexBufferBinding->setBinding(0, vertexBuffer);

    // Выставляем огромные границы, чтобы Ogre точно не отсек точки
    mBox.setExtents(Ogre::Vector3(-10000), Ogre::Vector3(10000));
  }

  HairRenderable::~HairRenderable() {}

  void HairRenderable::_updateRenderQueue(Ogre::RenderQueue* queue) { queue->addRenderable(this, mRenderQueueID); }

}  // namespace RenderHair::Render
