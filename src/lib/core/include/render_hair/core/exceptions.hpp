#pragma once

#include "render_hair/base/exceptions.hpp"

namespace RenderHair::Core {
class PhysicsException : public BaseException {
  using BaseException::BaseException;
};
}  // namespace RenderHair::Exception
