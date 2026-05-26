#include "render_hair/render/hair_model.hpp"

#include "render_hair/render/mesh_conversion.hpp"

namespace RenderHair::Render {

  HairModel::HairModel(Ogre::SceneManager& scene_manager, Ogre::SceneNode& parent_node, Ogre::MeshPtr mesh,
                       std::span<const HairRoot> roots) {
    const size_t hair_count = roots.size();
    if (hair_count == 0) { return; }

    const size_t points_per_hair = 3;
    const size_t total_vertex_count = hair_count * points_per_hair;

    auto triangles = RenderHair::Render::convert_mesh_to_triangles(mesh);
    std::vector<Ogre::Vector4> cpu_positions(total_vertex_count);

    // Длина одного сегмента волоса (можешь вынести в параметры)
    const float segment_length = 0.25F;

    for (size_t i = 0; i < hair_count; ++i) {
      const auto& root = roots[i];

      // Вычисляем базовый индекс в массиве для текущего волоса
      size_t base_idx = i * points_per_hair;

      if (root.triangle_index < 0 || root.triangle_index >= static_cast<int32_t>(triangles.size())) {
        for (size_t k = 0; k < points_per_hair; ++k) {
          cpu_positions[base_idx + k] = Ogre::Vector4(0, 0, 0, static_cast<float>(k));
        }
        continue;
      }

      const auto& tri = triangles[root.triangle_index];
      const Ogre::Vector3 A(tri.vertices[0].x, tri.vertices[0].y, tri.vertices[0].z);
      const Ogre::Vector3 B(tri.vertices[1].x, tri.vertices[1].y, tri.vertices[1].z);
      const Ogre::Vector3 C(tri.vertices[2].x, tri.vertices[2].y, tri.vertices[2].z);

      const Ogre::Vector3 nA(tri.normals[0].x, tri.normals[0].y, tri.normals[0].z);
      const Ogre::Vector3 nB(tri.normals[1].x, tri.normals[1].y, tri.normals[1].z);
      const Ogre::Vector3 nC(tri.normals[2].x, tri.normals[2].y, tri.normals[2].z);

      const float u = root.baricentric_coords.x;
      const float v = root.baricentric_coords.y;
      const float w = 1.0F - u - v;

      // Базовая точка на меше и нормаль
      const Ogre::Vector3 pos3D = (u * A) + (v * B) + (w * C);
      Ogre::Vector3 blended_normal = (u * nA) + (v * nB) + (w * nC);
      blended_normal.normalise();

      // Чуть приподнимаем корень над мешем от Z-fighting
      const Ogre::Vector3 root_pos = pos3D + (blended_normal * 0.001F);

      // Выращиваем 3 точки вдоль нормали
      for (size_t k = 0; k < points_per_hair; ++k) {
        Ogre::Vector3 p = root_pos + (blended_normal * (static_cast<float>(k) * segment_length));

        // В координту W записываем номер точки (0.0, 1.0 или 2.0)
        cpu_positions[base_idx + k] = Ogre::Vector4(p.x, p.y, p.z, static_cast<float>(k));
      }
    }

    // Создаем ручной меш
    const Ogre::MeshPtr hair_mesh = Ogre::MeshManager::getSingleton().createManual(
        "GeneratedHairMesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    Ogre::SubMesh* sub_mesh = hair_mesh->createSubMesh();
    sub_mesh->vertexData = new Ogre::VertexData();
    sub_mesh->vertexData->vertexCount = total_vertex_count;

    Ogre::VertexDeclaration* decl = sub_mesh->vertexData->vertexDeclaration;
    decl->addElement(0, 0, Ogre::VET_FLOAT4, Ogre::VES_POSITION);

    vertexBuffer_ = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
        decl->getVertexSize(0), sub_mesh->vertexData->vertexCount, Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
    vertexBuffer_->writeData(0, vertexBuffer_->getSizeInBytes(), cpu_positions.data(), true);
    sub_mesh->vertexData->vertexBufferBinding->setBinding(0, vertexBuffer_);

    const size_t index_count = hair_count * (points_per_hair + 1);
    sub_mesh->indexData = new Ogre::IndexData();
    sub_mesh->indexData->indexCount = index_count;

    Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
        Ogre::HardwareIndexBuffer::IT_32BIT, index_count, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    std::vector<uint32_t> indices(index_count);
    size_t idx_cursor = 0;
    for (size_t i = 0; i < hair_count; ++i) {
      auto base_vert = static_cast<uint32_t>(i * points_per_hair);

      indices[idx_cursor++] = base_vert;
      indices[idx_cursor++] = base_vert + 1;
      indices[idx_cursor++] = base_vert + 1;
      indices[idx_cursor++] = base_vert + 2;
    }
    ibuf->writeData(0, ibuf->getSizeInBytes(), indices.data(), true);
    sub_mesh->indexData->indexBuffer = ibuf;

    sub_mesh->operationType = Ogre::RenderOperation::OT_LINE_LIST;
    sub_mesh->useSharedVertices = false;

    hair_mesh->_setBounds(Ogre::AxisAlignedBox(Ogre::Vector3(-500, -500, -500), Ogre::Vector3(500, 500, 500)));

    entity_ = scene_manager.createEntity(hair_mesh);
    entity_->setMaterialName("MyRedPointMaterial");  // Переименуем его логически в шейдерах в HairMaterial

    node_ = parent_node.createChildSceneNode("HairModelNode");
    node_->attachObject(entity_);
  }

  HairModel::~HairModel() {
    if (node_ != nullptr && entity_ != nullptr) {
      Ogre::SceneManager* scnMgr = node_->getCreator();
      node_->detachObject(entity_);
      scnMgr->destroyEntity(entity_);
      scnMgr->destroySceneNode(node_);
      Ogre::MeshManager::getSingleton().remove("GeneratedHairMesh");
    }
  }

  Ogre::HardwareVertexBufferSharedPtr HairModel::getVertexBuffer() const { return vertexBuffer_; }

}  // namespace RenderHair::Render
