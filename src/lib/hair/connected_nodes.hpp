#ifndef RENDER_HAIR_CONNECTED_NODES_HPP
#define RENDER_HAIR_CONNECTED_NODES_HPP

#include <glm/glm.hpp>
#include <vector>

#include "node.hpp"

namespace RenderHair {

class ConnectedNodes {
  int nodesCount_{0};
  float segmentLength_{0.0F};

  float damping_{0.0F};
  int constraintIterations_{0};

  glm::vec3 rootPosition_{0.0F};

  std::vector<VerletNode> nodes_;

 public:
  struct Configuration {
    float total_length;
    int nodes_count;
    float damping;
    int constraintIterations;
  };

  explicit ConnectedNodes(const Configuration& config);

  void setup(const Configuration& config);

  void update(float delta_time, const glm::vec3& external_force);

  [[nodiscard]] int getNodesCount() const;

  // get position of specified node.
  [[nodiscard]] glm::vec3 getNodePosition(int index) const;

  [[nodiscard]] float getTotalLength() const;

  void setRootPosition(const glm::vec3& position);

 private:
  void applyConstraints();
};

}  // namespace RenderHair

#endif
