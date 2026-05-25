#include "connected_nodes.hpp"

#include "exceptions.hpp"
#include "physics_config.hpp"

namespace RenderHair {

ConnectedNodes::ConnectedNodes(const Configuration& config) { setup(config); }

void ConnectedNodes::setup(const Configuration& config) {
  if (config.nodes_count < 2) {
    throw Exception::PhysicsException{"Nodes count cannot be less than 2."};
  }
  nodesCount_ = config.nodes_count;
  segmentLength_ = config.total_length / static_cast<float>(nodesCount_);

  if (config.damping < EPS || config.damping > 1.0F) {
    throw Exception::PhysicsException{
        "The damping value should be in the range [0; 1]."};
  }
  damping_ = config.damping;

  if (config.constraintIterations < 0) {
    throw Exception::PhysicsException{
        "The constraint iterations value should be positive number."};
  }
  constraintIterations_ = config.constraintIterations;

  rootPosition_ = glm::vec3{0.0F};

  nodes_.clear();
  for (int i = 0; i < nodesCount_; ++i) {
    const glm::vec3 offset{0, 0, static_cast<float>(-i) * segmentLength_};
    const glm::vec3 pos = rootPosition_ + offset;
    nodes_.emplace_back(pos, pos, i == 0);
  }
}

void ConnectedNodes::update(float delta_time, const glm::vec3& external_force) {
  delta_time = std::min(delta_time, MINIMAL_DELTA_TIME);

  for (auto& node : nodes_) {
    if (node.isPinned) {
      node.currentPosition = rootPosition_;
      continue;
    }

    const auto velocity =
        (node.currentPosition - node.previousPosition) * damping_;
    node.previousPosition = node.currentPosition;
    node.currentPosition +=
        velocity + (external_force * delta_time * delta_time);
  }

  for (int i = 0; i < constraintIterations_; ++i) {
    applyConstraints();
  }
}

void ConnectedNodes::applyConstraints() {
  nodes_[0].currentPosition = rootPosition_;

  for (size_t i = 0; i < nodes_.size() - 1; ++i) {
    VerletNode& node1 = nodes_[i];
    VerletNode& node2 = nodes_[i + 1];

    const glm::vec3 delta = node2.currentPosition - node1.currentPosition;
    const float distance = glm::length(delta);

    if (distance < EPS) {
      continue;
    }

    const float error = (distance - segmentLength_) / distance;

    if (node1.isPinned) {
      node2.currentPosition -= delta * error;
    } else {
      const glm::vec3 correction = delta * 0.5F * error;
      node1.currentPosition += correction;
      node2.currentPosition -= correction;
    }
  }
}

int ConnectedNodes::getNodesCount() const { return nodesCount_; }

float ConnectedNodes::getTotalLength() const {
  return static_cast<float>(nodesCount_ - 1) * segmentLength_;
}

glm::vec3 ConnectedNodes::getNodePosition(int index) const {
  if (index < 0) {
    throw Exception::PhysicsException{"Node index out of range."};
  }
  const auto& target_node = nodes_[static_cast<size_t>(index)];
  return target_node.currentPosition;
}

void ConnectedNodes::setRootPosition(const glm::vec3& position) {
  rootPosition_ = position;
  nodes_[0].currentPosition = rootPosition_;
}

}  // namespace RenderHair
