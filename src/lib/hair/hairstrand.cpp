#include "hairstrand.hpp"

#include <algorithm>
#include <glm/geometric.hpp>

namespace RenderHair {

HairStrand::HairStrand(glm::vec3 rootPos, int segmentsCount,
                       float segmentLength)
    : segmentsCount_{segmentsCount},
      segmentLength_{segmentLength},
      rootPos_{rootPos} {
  for (int i = 0; i <= segmentsCount_; ++i) {
    glm::vec3 pos = rootPos_ + glm::vec3(0.0f, -i * segmentLength_, 0.0f);
    particles_.emplace_back(pos, (i == 0));
  }
}

void HairStrand::update(float dt, const glm::vec3& externalForce,
                        const std::vector<CollisionSphere>& spheres,
                        const std::vector<CollisionCube>& cubes) {
  dt = std::min(dt, 0.033f);

  for (auto& p : particles_) {
    if (p.isPinned) {
      p.position = rootPos_;
      continue;
    }

    glm::vec3 velocity = (p.position - p.oldPosition) * damping_;
    p.oldPosition = p.position;

    p.position += velocity + (externalForce * dt * dt);
  }

  for (int i = 0; i < constraintIterations_; ++i) {
    applyConstraints();
    handleCollisions(spheres, cubes);
  }
}

void HairStrand::applyConstraints() {
  particles_[0].position = rootPos_;

  for (size_t i = 0; i < particles_.size() - 1; ++i) {
    Particle& p1 = particles_[i];
    Particle& p2 = particles_[i + 1];

    glm::vec3 delta = p2.position - p1.position;
    float currentDist = glm::length(delta);

    if (currentDist == 0.0f) continue;

    float error = (currentDist - segmentLength_) / currentDist;

    if (p1.isPinned) {
      p2.position -= delta * error;
    } else {
      glm::vec3 correction = delta * 0.5f * error;
      p1.position += correction;
      p2.position -= correction;
    }
  }
}

void HairStrand::handleCollisions(const std::vector<CollisionSphere>& spheres,
                                  const std::vector<CollisionCube>& cubes) {
  for (auto& p : particles_) {
    if (p.isPinned) continue;

    for (const auto& sphere : spheres) {
      glm::vec3 delta = p.position - sphere.center;
      float dist = glm::length(delta);
      if (dist < sphere.radius && dist > 0.0001f) {
        p.position = sphere.center + (delta / dist) * sphere.radius;
      }
    }

    for (const auto& cube : cubes) {
      if (p.position.x >= cube.minBound.x && p.position.x <= cube.maxBound.x &&
          p.position.y >= cube.minBound.y && p.position.y <= cube.maxBound.y &&
          p.position.z >= cube.minBound.z && p.position.z <= cube.maxBound.z) {
        float dx1 = p.position.x - cube.minBound.x;
        float dx2 = cube.maxBound.x - p.position.x;
        float dy1 = p.position.y - cube.minBound.y;
        float dy2 = cube.maxBound.y - p.position.y;
        float dz1 = p.position.z - cube.minBound.z;
        float dz2 = cube.maxBound.z - p.position.z;

        float minDist = std::min({dx1, dx2, dy1, dy2, dz1, dz2});

        if (minDist == dx1)
          p.position.x = cube.minBound.x;
        else if (minDist == dx2)
          p.position.x = cube.maxBound.x;
        else if (minDist == dy1)
          p.position.y = cube.minBound.y;
        else if (minDist == dy2)
          p.position.y = cube.maxBound.y;
        else if (minDist == dz1)
          p.position.z = cube.minBound.z;
        else if (minDist == dz2)
          p.position.z = cube.maxBound.z;
      }
    }
  }
}

}  // namespace RenderHair
