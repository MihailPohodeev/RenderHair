#pragma once

#include "render_hair/base/exceptions.hpp"

namespace RenderHair::Error {
class PhysicsException : public BaseException {
  using BaseException::BaseException;
};
}  // namespace RenderHair::Exception
