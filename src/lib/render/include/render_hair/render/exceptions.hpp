#pragma once

#include "render_hair/base/exceptions.hpp"

namespace RenderHair::Render {

  class PhysicsException : public Base::Exception {
    using Exception::Exception;
  };
}  // namespace RenderHair::Render
