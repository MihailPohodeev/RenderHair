#include "render_hair/render/mesh_conversion.hpp"

#include <OgreSubMesh.h>

namespace RenderHair {
  std::vector<Triangle> convert_mesh_to_triangles(Ogre::MeshPtr mesh) {
    std::vector<Triangle> all_triangles;

    const size_t submesh_amount = mesh->getNumSubMeshes();
    for (unsigned short i = 0; i < submesh_amount; ++i) {
      Ogre::SubMesh *submesh = mesh->getSubMesh(i);
      Ogre::VertexData *vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
      if (vertex_data == nullptr) { continue; }

      auto *vertex_delcaration = vertex_data->vertexDeclaration;
      const Ogre::VertexElement *pos_elem = vertex_delcaration->findElementBySemantic(Ogre::VES_POSITION);
      const Ogre::VertexElement *norm_elem = vertex_delcaration->findElementBySemantic(Ogre::VES_NORMAL);
      const Ogre::VertexElement *uv_elem = vertex_delcaration->findElementBySemantic(Ogre::VES_TEXTURE_COORDINATES);

      if (pos_elem == nullptr) { continue; }

      std::vector<glm::vec3> local_positions(vertex_data->vertexCount, glm::vec3{});
      std::vector<glm::vec3> local_normals(vertex_data->vertexCount, glm::vec3{});
      std::vector<glm::vec2> local_uvs(vertex_data->vertexCount, glm::vec2{});

      auto expand_buffer = [vertex_data]<typename glm_vec>(const Ogre::VertexElement *element,
                                                           std::vector<glm_vec> &output_vec) {
        if (element == nullptr) { return; }
        const auto buf = vertex_data->vertexBufferBinding->getBuffer(element->getSource());
        auto *buf_start = static_cast<unsigned char *>(buf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        const size_t vertex_size = buf->getVertexSize();
        constexpr size_t dim = glm_vec::length();
        for (size_t j = 0; j < vertex_data->vertexCount; ++j) {
          float *pReal = nullptr;
          element->baseVertexPointerToElement(buf_start + (j * vertex_size), &pReal);
          glm_vec element_vec{};
          for (size_t i = 0; i < dim; ++i) { element_vec[i] = pReal[i]; }
          output_vec[j] = element_vec;
        }
        buf->unlock();
      };

      expand_buffer(pos_elem, local_positions);
      expand_buffer(norm_elem, local_normals);
      expand_buffer(uv_elem, local_uvs);

      Ogre::IndexData *indexData = submesh->indexData;
      if (indexData == nullptr || indexData->indexCount == 0) { continue; }

      Ogre::HardwareIndexBufferSharedPtr ibuf = indexData->indexBuffer;
      void *pIdx = ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);
      const size_t num_triangles = indexData->indexCount / 3;

      auto get_index = [&](size_t offset) -> uint32_t {
        if (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT) {
          return static_cast<uint32_t *>(pIdx)[offset];
        } else {
          return static_cast<uint16_t *>(pIdx)[offset];
        }
      };

      for (size_t triangle_num = 0; triangle_num < num_triangles; ++triangle_num) {
        const uint32_t idx0 = get_index(triangle_num * 3 + 0);
        const uint32_t idx1 = get_index(triangle_num * 3 + 1);
        const uint32_t idx2 = get_index(triangle_num * 3 + 2);

        Triangle tri;
        tri.submesh_index = i;
        tri.triangle_index = static_cast<int32_t>(triangle_num);

        tri.vertices = {local_positions[idx0], local_positions[idx1], local_positions[idx2]};
        tri.normals = {local_normals[idx0], local_normals[idx1], local_normals[idx2]};
        tri.texture_coords = {local_uvs[idx0], local_uvs[idx1], local_uvs[idx2]};

        all_triangles.push_back(tri);
      }
      ibuf->unlock();
    }

    return all_triangles;
  }

}  // namespace RenderHair
