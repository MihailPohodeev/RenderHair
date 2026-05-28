#pragma once

#include "render_hair/base/exceptions.hpp"

namespace RenderHair::Error {
  class RenderException : public BaseException {
    using BaseException::BaseException;
  };

  class PhysicsException : public BaseException {
    using BaseException::BaseException;
  };
}  // namespace RenderHair::Error
