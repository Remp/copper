#pragma once

#include "Copper/Platform.h"
#include "GLFW/glfw3.h"

namespace Copper
{

class MacOsPlatform : public Platform
{
private:
    inline virtual float GetTimeImpl() const override { return glfwGetTime(); }
};

}
