#ifndef HAIR_STRAND_HPP
#define HAIR_STRAND_HPP

#include <glm/vec3.hpp>
#include <vector>

namespace RenderHair {

struct CollisionSphere {
  glm::vec3 center;
  float radius;
};

struct CollisionCube {
  glm::vec3 minBound;
  glm::vec3 maxBound;
};

struct Particle {
  glm::vec3 position;
  glm::vec3 oldPosition;
  bool isPinned;

  Particle(glm::vec3 pos, bool pinned = false)
      : position(pos), oldPosition(pos), isPinned(pinned) {}
};

class HairStrand {
 public:
  HairStrand(glm::vec3 rootPos, int segmentsCount, float segmentLength);

  void update(float dt, const glm::vec3& externalForce,
              const std::vector<CollisionSphere>& spheres,
              const std::vector<CollisionCube>& cubes);
  const std::vector<Particle>& getParticles() const { return particles_; }
  void setRootPosition(const glm::vec3& newPos) { rootPos_ = newPos; }

 private:
  void applyConstraints();
  void handleCollisions(const std::vector<CollisionSphere>& spheres,
                        const std::vector<CollisionCube>& cubes);

  std::vector<Particle> particles_;
  int segmentsCount_;
  float segmentLength_;
  glm::vec3 rootPos_;

  float damping_ = 0.98f;
  int constraintIterations_ = 5;
};
}  // namespace RenderHair

#endif  // HAIRSTRAND_HPP
