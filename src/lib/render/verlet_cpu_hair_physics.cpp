#include "render_hair/render/verlet_cpu_hair_physics.hpp"

#include <cstring>
#include <iostream>

#include "render_hair/render/collision/collider_manager.hpp"
#include "render_hair/render/exceptions.hpp"

namespace RenderHair {

  VerletCPU_HairPhysics::VerletCPU_HairPhysics(Ogre::HardwareVertexBufferSharedPtr vertex_buffer,
                                               const HairPhysicsSettings& settings,
                                               const std::vector<Ogre::Vector4>& init_nodes)
      : vertexBuffer_{vertex_buffer},
        settings_{settings} {
    if (!vertexBuffer_) { throw Error::PhysicsException{"Vertex buffer is `nullptr`."}; }

    totalNodes_ = vertexBuffer_->getNumVertices();
    hairCount_ = totalNodes_ / settings_.nodes_per_hair;

    currentPositionsCache_.resize(totalNodes_);
    previousPositions_.resize(totalNodes_);
    localRootsCache_.resize(hairCount_);

    void* gpu_mem = vertexBuffer_->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);
    std::memcpy(currentPositionsCache_.data(), gpu_mem, vertexBuffer_->getSizeInBytes());
    vertexBuffer_->unlock();

    for (size_t i = 0; i < totalNodes_; ++i) { previousPositions_[i] = currentPositionsCache_[i].xyz(); }

    for (size_t i = 0; i < hairCount_; ++i) {
      const size_t root_idx = i * settings_.nodes_per_hair;
      localRootsCache_[i] = Ogre::Vector3(currentPositionsCache_[root_idx].x, currentPositionsCache_[root_idx].y,
                                          currentPositionsCache_[root_idx].z);
    }
  }

  void VerletCPU_HairPhysics::update(float delta_time, const Ogre::Matrix4& world_matrix) {
    if (!vertexBuffer_ || currentPositionsCache_.empty()) return;

    const float dt = std::max(delta_time, 0.001f);

    if (isFirstFrame_) {
      for (size_t i = 0; i < totalNodes_; ++i) {
        Ogre::Vector4 world_node = world_matrix * Ogre::Vector4{currentPositionsCache_[i].xyz(), 1.0F};
        currentPositionsCache_[i].x = world_node.x;
        currentPositionsCache_[i].y = world_node.y;
        currentPositionsCache_[i].z = world_node.z;
        previousPositions_[i] = Ogre::Vector3(world_node.x, world_node.y, world_node.z);
      }
      isFirstFrame_ = false;
    }

    // for (size_t i = 0; i < hairCount_; ++i) {
    //   const size_t root_idx = i * settings_.nodes_per_hair;
    //   Ogre::Vector4 res_pos = world_matrix * Ogre::Vector4{localRootsCache_[i], 1.0F};

    //   currentPositionsCache_[root_idx].x = res_pos.x;
    //   currentPositionsCache_[root_idx].y = res_pos.y;
    //   currentPositionsCache_[root_idx].z = res_pos.z;
    // }

    // const Ogre::Vector3 gravity(0.0f, -9.81f, 0.0f);
    // const float damping = 0.97f;

    std::vector<Ogre::Vector4> temp_current{currentPositionsCache_};

    // for (size_t i = 0; i < hairCount_; ++i) {
    //   for (size_t k = 1; k < settings_.nodes_per_hair; ++k) {
    //     const size_t index = (i * settings_.nodes_per_hair) + k;

    //     Ogre::Vector3 current_pos{currentPositionsCache_[index].xyz()};
    //     Ogre::Vector3 prev_pos{previousPositions_[index]};

    //     Ogre::Vector3 velocity = (current_pos - prev_pos) * damping;
    //     Ogre::Vector3 next_pos = current_pos + velocity + (gravity * dt * dt);

    //     float current_pos_w = currentPositionsCache_[index].w;
    //     currentPositionsCache_[index] = Ogre::Vector4{next_pos, current_pos_w};
    //   }
    // }

    // resolveCollisions();

    // const float scale_coefficient = extract_axisX_scale_coefficient(world_matrix);
    // const float target_length =
    //     (settings_.one_hair_length * scale_coefficient) / static_cast<float>(settings_.nodes_per_hair - 1);
    // const int constraint_iterations = 5;
    // const float stiffness = 0.85f;
    // relaxationConstraints(target_length, constraint_iterations, stiffness);

    for (size_t i = 0; i < totalNodes_; ++i) { previousPositions_[i] = temp_current[i].xyz(); }

    void* gpu_mem = vertexBuffer_->lock(Ogre::HardwareBuffer::HBL_DISCARD);
    std::memcpy(gpu_mem, currentPositionsCache_.data(), vertexBuffer_->getSizeInBytes());
    vertexBuffer_->unlock();
  }

  void VerletCPU_HairPhysics::relaxationConstraints(float target_length, int constraint_iterations, float stiffness) {
    for (int iter = 0; iter < constraint_iterations; ++iter) {
      for (size_t i = 0; i < hairCount_; ++i) {
        for (size_t k = 0; k < settings_.nodes_per_hair - 1; ++k) {
          const size_t idxA = (i * settings_.nodes_per_hair) + k;
          const size_t idxB = idxA + 1;

          Ogre::Vector3 pA(currentPositionsCache_[idxA].x, currentPositionsCache_[idxA].y,
                           currentPositionsCache_[idxA].z);
          Ogre::Vector3 pB(currentPositionsCache_[idxB].x, currentPositionsCache_[idxB].y,
                           currentPositionsCache_[idxB].z);

          Ogre::Vector3 delta = pB - pA;
          float current_length = delta.length();

          if (current_length < 0.0001f) continue;

          float diff = target_length - current_length;
          float percent = (diff / current_length) * 0.5f * stiffness;
          Ogre::Vector3 correction = delta * percent;

          if (k == 0) {
            pB += correction * 2.0f;
          } else {
            pA -= correction;
            pB += correction;
          }

          if (k > 0) {
            currentPositionsCache_[idxA].x = pA.x;
            currentPositionsCache_[idxA].y = pA.y;
            currentPositionsCache_[idxA].z = pA.z;
          }
          currentPositionsCache_[idxB].x = pB.x;
          currentPositionsCache_[idxB].y = pB.y;
          currentPositionsCache_[idxB].z = pB.z;
        }
      }
    }
  }

  float VerletCPU_HairPhysics::extract_axisX_scale_coefficient(const Ogre::Matrix4& matrix) {
    const Ogre::Vector3 axisX(matrix[0][0], matrix[1][0], matrix[2][0]);
    return axisX.length();
  }

  void VerletCPU_HairPhysics::resolveCollisions() {
    // Получаем список всех зарегистрированных коллайдеров
    const auto colliders = ColliderManager::getInstance().getColliders();

    // Небольшой отступ (skin width), чтобы волосы не залипали ровно на границе коллайдера
    const float skin_offset = 0.005f;

    for (const auto& collider : colliders) {
      // Проверяем, является ли коллайдер сферой
      if (auto sphere = std::get_if<Collider::Sphere>(&collider)) {
        const Ogre::Vector3 sphere_center = sphere->center;

        const float min_distance = sphere->radius + skin_offset;
        const float min_distance_sq = min_distance * min_distance;

        // Перебираем все узлы волос
        for (size_t i = 0; i < hairCount_; ++i) {
          // Начинаем с k = 1, так как корень (k = 0) жестко привязан к голове
          for (size_t k = 1; k < settings_.nodes_per_hair; ++k) {
            const size_t index = (i * settings_.nodes_per_hair) + k;

            Ogre::Vector3 node_pos{currentPositionsCache_[index].xyz()};
            Ogre::Vector3 to_node = node_pos - sphere_center;
            float dist_sq = to_node.squaredLength();

            // Если квадрат расстояния меньше квадрата радиуса — точка внутри сферы
            if (dist_sq < min_distance_sq) {
              float dist = std::sqrt(dist_sq);

              // Защита от деления на ноль, если точка оказалась ровно в центре сферы
              Ogre::Vector3 normal = (dist > 0.0001f) ? (to_node / dist) : Ogre::Vector3::UNIT_Y;

              // Выталкиваем точку на поверхность сферы
              Ogre::Vector3 corrected_pos = sphere_center + normal * min_distance;

              // Обновляем кэш позиций, сохраняя координату W
              currentPositionsCache_[index].x = corrected_pos.x;
              currentPositionsCache_[index].y = corrected_pos.y;
              currentPositionsCache_[index].z = corrected_pos.z;
            }
          }
        }
      }
      // Сюда можно будет добавить обработку Collider::Capsule через else if
    }
  }

}  // namespace RenderHair
