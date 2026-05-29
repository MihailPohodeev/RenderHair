#pragma once

#include <OgreLogManager.h>
#include <OgreMesh.h>
#include <OgreMeshManager.h>
#include <OgreSceneNode.h>
#include <OgreSubEntity.h>
#include <OgreSubMesh.h>
#include <OgreTechnique.h>

#include <cstdint>
#include <format>
#include <memory>

#include "render_hair/render/hair_physics.hpp"
#include "render_hair/render/hair_roots_generator.hpp"

namespace RenderHair {
  template <HairPhysicsConcept PhysicsType>
  class HairInstance {
    std::unique_ptr<PhysicsType> physics_{nullptr};
    Ogre::HardwareVertexBufferSharedPtr vertexBuffer_;
    Ogre::Entity* entity_;
    int32_t nodesPerOneHair_{2};
    float oneHairLength_{0.0F};
    Ogre::SceneNode& sceneNode_;

   public:
    struct Settings {
      int32_t nodes_per_hair{2};
      float one_hair_length{0.0F};
      Ogre::SceneManager* scene_manager{nullptr};
      Ogre::SceneNode* target_node{nullptr};
      Ogre::MeshPtr mesh{nullptr};
    };

    explicit HairInstance(const Settings& settings)
        : nodesPerOneHair_{settings.nodes_per_hair},
          oneHairLength_{settings.one_hair_length},
          sceneNode_{*settings.target_node} {
      std::vector<Ogre::Vector4> hair_nodes =
          HairRootsGenerator::generateWithMap(settings.mesh, nodesPerOneHair_, oneHairLength_, 8096);

      Ogre::LogManager::getSingleton().logMessage(std::format("Hairs count = {}", hair_nodes.size()));

      if (hair_nodes.empty()) { return; }

      const size_t total_nodes_count = hair_nodes.size();
      const size_t hair_count = total_nodes_count / static_cast<size_t>(nodesPerOneHair_);

      vertexBuffer_ = allocateVertexBuffer(total_nodes_count);
      vertexBuffer_->writeData(0, vertexBuffer_->getSizeInBytes(), hair_nodes.data(), true);

      Ogre::MeshPtr hair_mesh = Ogre::MeshManager::getSingleton().createManual(
          "GeneratedHairMesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

      Ogre::SubMesh* sub_mesh = hair_mesh->createSubMesh();
      sub_mesh->vertexData = new Ogre::VertexData();
      sub_mesh->vertexData->vertexCount = total_nodes_count;

      Ogre::VertexDeclaration* decl = sub_mesh->vertexData->vertexDeclaration;
      decl->addElement(0, 0, Ogre::VET_FLOAT4, Ogre::VES_POSITION);

      sub_mesh->vertexData->vertexBufferBinding->setBinding(0, vertexBuffer_);

      const auto segments_per_hair = static_cast<size_t>(nodesPerOneHair_ - 1);
      const size_t index_count = hair_count * segments_per_hair * 2;

      sub_mesh->indexData = new Ogre::IndexData();
      sub_mesh->indexData->indexCount = index_count;

      Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
          Ogre::HardwareIndexBuffer::IT_32BIT, index_count, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

      std::vector<uint32_t> indices(index_count);
      size_t idx_cursor = 0;

      for (size_t i = 0; i < hair_count; ++i) {
        const auto base_vert = static_cast<uint32_t>(i * nodesPerOneHair_);

        // Связываем последовательно пары точек (0->1, 1->2...) для каждого волоска
        for (size_t k = 0; k < segments_per_hair; ++k) {
          indices[idx_cursor++] = base_vert + k;
          indices[idx_cursor++] = base_vert + k + 1;
        }
      }

      // Загружаем индексы на видеокарту
      ibuf->writeData(0, ibuf->getSizeInBytes(), indices.data(), true);
      sub_mesh->indexData->indexBuffer = ibuf;

      // Указываем топологию отрисовки линий и отключаем общие вершины
      sub_mesh->operationType = Ogre::RenderOperation::OT_LINE_LIST;
      sub_mesh->useSharedVertices = false;

      // Задаем границы видимости (Bounding Box), чтобы меш не исчезал при повороте камеры
      hair_mesh->_setBounds(Ogre::AxisAlignedBox(Ogre::Vector3(-500, -500, -500), Ogre::Vector3(500, 500, 500)));

      // 6. Создаем Entity и привязываем к графической сцене Ogre
      // ВАЖНО: Делаем ноду волос независимой от ноды-родителя,
      // чтобы избежать повторного наложения масштаба (Scale = 50) при рендеринге мировых координат
      entity_ = settings.scene_manager->createEntity(hair_mesh);
      entity_->setMaterialName("MyHairMaterial");
      auto params =
          entity_->getSubEntity(0)->getMaterial()->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
      params->setNamedConstant("nodes_per_hair", settings.nodes_per_hair);
      params->setNamedConstant("top_color", Ogre::Vector3(0.0F, 0.0F, 0.0F));
      params->setNamedConstant("down_color", Ogre::Vector3(0.24F, 0.13F, 0.08F));

      settings.target_node->attachObject(entity_);

      // 7. Наконец, создаем экземпляр физики, отдавая ему готовый буфер памяти
      physics_ = std::make_unique<PhysicsType>(
          vertexBuffer_,
          HairPhysicsSettings{.nodes_per_hair = settings.nodes_per_hair, .one_hair_length = settings.one_hair_length});

      // Передаем физическому движку стартовые позы для инициализации кэшей Верле
      // physics_->initNodes(hair_nodes, nodesPerOneHair_);
    }

    void update(float delta_time) {
      Ogre::Matrix4 world_matrix = sceneNode_._getFullTransform();
      if (physics_ == nullptr) { return; }
      physics_->update(delta_time, world_matrix);
    }

   private:
    Ogre::HardwareVertexBufferSharedPtr allocateVertexBuffer(size_t total_nodes) {
      constexpr size_t size_of_one_node = sizeof(Ogre::Vector4);
      return Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(size_of_one_node, total_nodes,
                                                                            Ogre::HardwareBuffer::HBU_DYNAMIC);
    }
  };
}  // namespace RenderHair
