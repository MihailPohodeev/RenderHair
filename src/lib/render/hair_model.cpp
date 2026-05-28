#include "render_hair/render/hair_model.hpp"

namespace RenderHair {

  HairModel::HairModel(Ogre::SceneNode& parent_node, Ogre::MeshPtr mesh, Ogre::SceneManager& scene_manager,
                       Ogre::HardwareVertexBufferSharedPtr exаternal_buffer, const Settings& settings)
      : vertexBuffer_{exаternal_buffer},
        settings_{settings} {
#if 0
    auto triangles = RenderHair::convert_mesh_to_triangles(mesh);

    const size_t points_per_hair = settings.points_per_strand;
    const size_t total_vertex_count = hair_count * points_per_hair;

    std::vector<Ogre::Vector4> cpu_positions(total_vertex_count);
    const float segment_length = settings.one_strand_length / (points_per_hair - 1);

    for (size_t i = 0; i < hair_count; ++i) {
      const auto& root = roots[i];
      const size_t base_idx = i * points_per_hair;

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

      // Выращиваем точки вдоль нормали
      for (size_t k = 0; k < points_per_hair; ++k) {
        const Ogre::Vector3 next_particle = root_pos + (blended_normal * (static_cast<float>(k) * segment_length));
        cpu_positions[base_idx + k] =
            Ogre::Vector4(next_particle.x, next_particle.y, next_particle.z, static_cast<float>(k));
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

    const size_t segments_per_hair = points_per_hair - 1;
    const size_t index_count = hair_count * segments_per_hair * 2;

    sub_mesh->indexData = new Ogre::IndexData();
    sub_mesh->indexData->indexCount = index_count;

    Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
        Ogre::HardwareIndexBuffer::IT_32BIT, index_count, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    std::vector<uint32_t> indices(index_count);
    size_t idx_cursor = 0;

    for (size_t i = 0; i < hair_count; ++i) {
      auto base_vert = static_cast<uint32_t>(i * points_per_hair);

      // Идем по всем сегментам внутри одного волоска
      for (size_t k = 0; k < segments_per_hair; ++k) {
        indices[idx_cursor++] = base_vert + k;
        indices[idx_cursor++] = base_vert + k + 1;
      }
    }
    ibuf->writeData(0, ibuf->getSizeInBytes(), indices.data(), true);
    sub_mesh->indexData->indexBuffer = ibuf;

    sub_mesh->operationType = Ogre::RenderOperation::OT_LINE_LIST;
    sub_mesh->useSharedVertices = false;

    hair_mesh->_setBounds(Ogre::AxisAlignedBox(Ogre::Vector3(-500, -500, -500), Ogre::Vector3(500, 500, 500)));

    entity_ = scene_manager.createEntity(hair_mesh);
    entity_->setMaterialName("MyRedPointMaterial");

    node_ = parent_node.createChildSceneNode("HairModelNode");
    node_->attachObject(entity_);

    cpu_positions_cache_ = cpu_positions;
    previous_positions_.resize(total_vertex_count);
    for (size_t i = 0; i < total_vertex_count; ++i) {
      previous_positions_[i] = Ogre::Vector3(cpu_positions[i].x, cpu_positions[i].y, cpu_positions[i].z);
    }

    local_roots_cache_.resize(hair_count);
    for (size_t i = 0; i < hair_count; ++i) {
      // k = 0 — это корень. Берем его начальные сгенерированные координаты
      size_t root_idx = i * points_per_hair;
      local_roots_cache_[i] =
          Ogre::Vector3(cpu_positions[root_idx].x, cpu_positions[root_idx].y, cpu_positions[root_idx].z);
    }
#endif
  }

  HairModel::~HairModel() {}

  // void HairModel::update(float delta_time, const Ogre::Matrix4& model_mat) {
  //   if (cpu_positions_cache_.empty() || !vertexBuffer_) { return; }

  //   // Ограничиваем шаг времени, чтобы физика вела себя стабильно даже при просадках FPS
  //   const float dt = std::min(delta_time, 0.02f);
  //   if (dt < 0.0001f) { return; }

  //   const size_t points_per_hair = settings_.points_per_strand;
  //   if (points_per_hair < 2) { return; }

  //   const size_t hair_count = cpu_positions_cache_.size() / points_per_hair;
  //   const float target_length = settings_.one_strand_length / static_cast<float>(points_per_hair - 1);

  //   // Гравитация в мировых координатах (тянет строго вниз по оси Y)
  //   const Ogre::Vector3 gravity(0.0f, -9.81f, 0.0f);

  //   // Сопротивление воздуха (затухание скорости), чтобы волосы не качались бесконечно
  //   const float damping = settings_.damping;

  //   // === TODO 1-2: Подгоняем корни под положение сетки и заносим в буфер кэша ===
  //   for (size_t i = 0; i < hair_count; ++i) {
  //     const size_t root_idx = i * points_per_hair;

  //     // Переводим локальную координату корня из геометрии сферы в Мировые Координаты.
  //     // transformAffine правильно учитывает Позицию, Поворот и Масштаб (Scale = 50) сферы.
  //     Ogre::Vector3 world_root_pos = model_mat * local_roots_cache_[i];

  //     // Записываем новую позицию корня в наш кэш
  //     cpu_positions_cache_[root_idx].x = world_root_pos.x;
  //     cpu_positions_cache_[root_idx].y = world_root_pos.y;
  //     cpu_positions_cache_[root_idx].z = world_root_pos.z;

  //     // Предыдущая позиция корня всегда равна текущей, чтобы у корней не накапливалась скорость
  //     previous_positions_[root_idx] = world_root_pos;
  //   }

  //   // === TODO 3: Пересчитываем физику остальных частиц (Шаг Интеграции Верле) ===
  //   for (size_t i = 0; i < hair_count; ++i) {
  //     // k = 0 (корень) мы обработали выше, поэтому симулируем частицы начиная с k = 1
  //     for (size_t k = 1; k < points_per_hair; ++k) {
  //       const size_t idx = (i * points_per_hair) + k;

  //       Ogre::Vector3 current_pos(cpu_positions_cache_[idx].x, cpu_positions_cache_[idx].y,
  //                                 cpu_positions_cache_[idx].z);
  //       Ogre::Vector3 prev_pos = previous_positions_[idx];

  //       // Скорость — это разность текущего и прошлого положения
  //       Ogre::Vector3 velocity = (current_pos - prev_pos) * damping;

  //       // Формула Верле: x_next = x_curr + velocity + acceleration * dt^2
  //       Ogre::Vector3 next_pos = current_pos + velocity + (gravity * dt * dt);

  //       // Старая позиция уходит в историю
  //       previous_positions_[idx] = current_pos;

  //       // Новая позиция записывается в кэш
  //       cpu_positions_cache_[idx].x = next_pos.x;
  //       cpu_positions_cache_[idx].y = next_pos.y;
  //       cpu_positions_cache_[idx].z = next_pos.z;
  //     }
  //   }

  //   // === Удовлетворение связей (Constraints) ===
  //   // Подтягиваем частицы друг к другу, чтобы волос сохранял свою длину.
  //   const int constraint_iterations = 8;
  //   for (int iter = 0; iter < constraint_iterations; ++iter) {
  //     for (size_t i = 0; i < hair_count; ++i) {
  //       for (size_t k = 0; k < points_per_hair - 1; ++k) {
  //         const size_t idxA = (i * points_per_hair) + k;
  //         const size_t idxB = idxA + 1;

  //         Ogre::Vector3 pA(cpu_positions_cache_[idxA].x, cpu_positions_cache_[idxA].y, cpu_positions_cache_[idxA].z);
  //         Ogre::Vector3 pB(cpu_positions_cache_[idxB].x, cpu_positions_cache_[idxB].y, cpu_positions_cache_[idxB].z);

  //         Ogre::Vector3 delta = pB - pA;
  //         float current_length = delta.length();

  //         if (current_length < 0.0001f) { continue; }

  //         float diff = target_length - current_length;
  //         float percent = (diff / current_length) * 0.5f;
  //         Ogre::Vector3 correction = delta * percent;

  //         // Так как корень (k=0) зафиксирован на сфере, всю ошибку длины берет на себя следующая точка
  //         if (k == 0) {
  //           pB += correction * 2.0f;
  //         } else {
  //           pA -= correction;
  //           pB += correction;
  //         }

  //         if (k > 0) {
  //           cpu_positions_cache_[idxA].x = pA.x;
  //           cpu_positions_cache_[idxA].y = pA.y;
  //           cpu_positions_cache_[idxA].z = pA.z;
  //         }
  //         cpu_positions_cache_[idxB].x = pB.x;
  //         cpu_positions_cache_[idxB].y = pB.y;
  //         cpu_positions_cache_[idxB].z = pB.z;
  //       }
  //     }
  //   }

  //   // === TODO 4: Пишем измененные позиции в буфер (Загрузка на GPU) ===
  //   void* gpu_mem = vertexBuffer_->lock(Ogre::HardwareBuffer::HBL_DISCARD);
  //   std::memcpy(gpu_mem, cpu_positions_cache_.data(), vertexBuffer_->getSizeInBytes());
  //   vertexBuffer_->unlock();
  // }

  Ogre::HardwareVertexBufferSharedPtr HairModel::getVertexBuffer() const { return vertexBuffer_; }

  Ogre::SceneNode* HairModel::getSceneNode() const { return node_; }

}  // namespace RenderHair
