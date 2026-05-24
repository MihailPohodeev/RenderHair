#ifndef RENDER_HAIR_APP_AUXILIARY_HPP
#define RENDER_HAIR_APP_AUXILIARY_HPP

#include <OgreMesh.h>
#include <OgreSubMesh.h>

#include "hair_roots_allocation.hpp"

namespace RenderHair::App {
inline std::vector<Triangle> convertMeshToTriangles(const Ogre::Mesh& mesh) {
  std::vector<RenderHair::Triangle> allTriangles;

  for (unsigned short i = 0; i < mesh.getNumSubMeshes(); ++i) {
    const Ogre::SubMesh* submesh = mesh.getSubMesh(i);

    const Ogre::VertexData* vertexData = submesh->useSharedVertices
                                             ? mesh.sharedVertexData
                                             : submesh->vertexData;
    const Ogre::IndexData* indexData = submesh->indexData;

    if (!vertexData || !indexData) continue;

    const Ogre::VertexElement* posElem =
        vertexData->vertexDeclaration->findElementBySemantic(
            Ogre::VES_POSITION);
    const Ogre::VertexElement* normElem =
        vertexData->vertexDeclaration->findElementBySemantic(Ogre::VES_NORMAL);
    const Ogre::VertexElement* uvElem =
        vertexData->vertexDeclaration->findElementBySemantic(
            Ogre::VES_TEXTURE_COORDINATES, 0);

    Ogre::HardwareVertexBufferSharedPtr vBuf =
        vertexData->vertexBufferBinding->getBuffer(posElem->getSource());
    Ogre::HardwareIndexBufferSharedPtr iBuf = indexData->indexBuffer;

    auto* vertexPtr = static_cast<unsigned char*>(
        vBuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
    auto* indexPtr = iBuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);

    bool use32BitIndices =
        (iBuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
    size_t numIndices = indexData->indexCount;

    allTriangles.reserve(allTriangles.size() + (numIndices / 3));

    for (size_t j = 0; j < numIndices; j += 3) {
      RenderHair::Triangle tri;

      for (size_t v = 0; v < 3; ++v) {
        uint32_t vertexIdx;
        if (use32BitIndices) {
          vertexIdx = static_cast<uint32_t*>(indexPtr)[j + v];
        } else {
          vertexIdx = static_cast<uint16_t*>(indexPtr)[j + v];
        }

        unsigned char* pVertex = vertexPtr + vertexIdx * vBuf->getVertexSize();

        float* pRawData;
        posElem->baseVertexPointerToElement(pVertex, &pRawData);
        tri.vertexes[v] = glm::vec3(pRawData[0], pRawData[1], pRawData[2]);

        normElem->baseVertexPointerToElement(pVertex, &pRawData);
        tri.normals[v] = glm::vec3(pRawData[0], pRawData[1], pRawData[2]);

        uvElem->baseVertexPointerToElement(pVertex, &pRawData);
        tri.uv[v] = glm::vec2(pRawData[0], pRawData[1]);
      }
      allTriangles.push_back(tri);
    }

    vBuf->unlock();
    iBuf->unlock();
  }

  return allTriangles;
}
}  // namespace RenderHair::App

#endif  // RENDER_HAIR_AUXILIARY_HPP
