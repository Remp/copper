#include "cprpch.h"
#include "MacOsWindow.h"
#include "Copper/Core/Log.h"
#include "Copper/Event/MouseEvent.h"
#include "Copper/Event/ApplicationEvent.h"
#include "Copper/Event/KeyEvent.h"
#include "MacOsInput.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "glad/glad.h"

namespace Copper
{

static bool s_GLFWInitialized = false;

Window* Window::Create(const WindowProps& props)
{
    return new MacOsWindow(props);
}

MacOsWindow::MacOsWindow(const WindowProps& props)
{
    Init(props);
}

MacOsWindow::~MacOsWindow()
{
    Shutdown();
}

void MacOsWindow::Init(const WindowProps& props)
{
    m_Data.Title = props.Title;
    m_Data.Width = props.Width;
    m_Data.Height = props.Height;

    CPR_CORE_INFO("Creating mainwindow {0} ({1}, {2})", m_Data.Title, m_Data.Width, m_Data.Height);

    if (!s_GLFWInitialized)
    {
        int success = glfwInit();

        CPR_CORE_ASSERT(success, "Could not initialize GLFW");

        s_GLFWInitialized = true;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);

    m_Window = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Data.Title.c_str(), NULL, nullptr);
    m_GraphicContext = std::make_unique<OpenGLContext>(m_Window);
    m_Input = std::make_unique<MacOsInput>(m_Window);

    // initialize framebuffer size
    glfwGetFramebufferSize(m_Window, (int*)&m_Data.FbWidth, (int*)&m_Data.FbHeight);

    glfwSetWindowUserPointer(m_Window, &m_Data);

    glfwSetErrorCallback(&MacOsWindow::OnGLFWError);
    glfwSetMouseButtonCallback(m_Window, &MacOsWindow::OnGLFWMouseButton);
    glfwSetScrollCallback(m_Window, &MacOsWindow::OnGLFWMouseScroll);
    glfwSetKeyCallback(m_Window, &MacOsWindow::OnGLFWKey);
    glfwSetWindowCloseCallback(m_Window, &MacOsWindow::OnGLFWWindowClose);
    glfwSetWindowSizeCallback(m_Window, &MacOsWindow::OnGLFWWindowResize);
    glfwSetCursorPosCallback(m_Window, &MacOsWindow::OnGLFWMouseMove);
    glfwSetFramebufferSizeCallback(m_Window, &MacOsWindow::OnGLFWWindowFbResize);

    SetVSync(true);
}

void MacOsWindow::OnUpdate()
{
    glfwPollEvents();
    m_GraphicContext->SwapBuffer();
}

void MacOsWindow::SetVSync(bool enabled)
{
    glfwSwapInterval(enabled ? 1 : 0);
    m_Data.VSync = enabled;
}

bool MacOsWindow::IsVSync() const
{
    return m_Data.VSync;
}

void MacOsWindow::SetEventCallback(const Window::EventCallbackFn& callback)
{
    m_Data.EventCallback = callback;
}

void MacOsWindow::OnGLFWMouseButton(GLFWwindow* window, int button, int action, int /*mode*/)
{
    WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

    switch (action) {
    case GLFW_PRESS:
    {
        MouseButtonPressedEvent event(button);
        data.EventCallback(event);
        break;
    }

    case GLFW_RELEASE:
    {
        MouseButtonReleasedEvent event(button);
        data.EventCallback(event);
        break;
    }

    default:
        CPR_CORE_WARN("MacOsWindow::OnGLFWMouseButton unhandled event: {0} {1} {2}", button, action);
    }
}

void MacOsWindow::OnGLFWMouseMove(GLFWwindow* window, double x, double y)
{
    WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

    MouseMovedEvent event(x, y);
    data.EventCallback(event);
}

void MacOsWindow::OnGLFWMouseScroll(GLFWwindow* window, double offsetX, double offsetY)
{
    WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

    MouseScrollEvent event(offsetX, offsetY);
    data.EventCallback(event);
}

void MacOsWindow::OnGLFWWindowClose(GLFWwindow* window)
{
    WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

    WindowCloseEvent event;
    data.EventCallback(event);
}

void MacOsWindow::OnGLFWWindowResize(GLFWwindow* window, int width, int height)
{
    WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

    data.Width = width;
    data.Height = height;

    WindowResizeEvent event(width, height);
    data.EventCallback(event);
}

void MacOsWindow::OnGLFWWindowFbResize(GLFWwindow* window, int width, int height)
{
    WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

    data.FbWidth = width;
    data.FbHeight = height;

    WindowFBResizeEvent event(width, height);
    data.EventCallback(event);
}

void MacOsWindow::OnGLFWKey(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
    WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

    switch (action) {
    case GLFW_PRESS:
    {
        KeyPressedEvent event(key, 0);
        data.EventCallback(event);
        break;
    }

    case GLFW_RELEASE:
    {
        KeyReleasedEvent event(key);
        data.EventCallback(event);
        break;
    }

    case GLFW_REPEAT:
    {
        KeyPressedEvent event(key, 1);
        data.EventCallback(event);
        break;
    }

    default:
        CPR_CORE_WARN("MacOsWindow::OnGLFWKey unhandled event: {0} {1}", key, action);
    }
}

void MacOsWindow::OnGLFWError(int error, const char* description)
{
    CPR_CORE_ERROR("MacOsWindow::OnGLFWError: {0}, {1}", error, description);
}

std::pair<float, float> MacOsWindow::GetDPI() const
{
    // TODO: discover how to render with proper DPI when the window is splitted between 2 monitors
    float x = 0;
    float y = 0;

    glfwGetMonitorContentScale(GetCurrentMonitor(), &x, &y);

    return std::make_pair(x, y);
}

GLFWmonitor* MacOsWindow::GetCurrentMonitor() const
{
    int wPosX = 0;
    int wPosY = 0;
    int wWidth = 0;
    int wHeight = 0;
    int monitorsCount = 0;

    glfwGetWindowPos(m_Window, &wPosX, &wPosY);
    glfwGetWindowSize(m_Window, &wWidth, &wHeight);

    GLFWmonitor** monitors = glfwGetMonitors(&monitorsCount);

    int bestOverlap = 0;
    GLFWmonitor* bestMonitor = monitorsCount > 0 ? monitors[0] : nullptr;

    for (int i = 0; i < monitorsCount; ++i)
    {
        int mX = 0;
        int mY = 0;

        glfwGetMonitorPos(monitors[i], &mX, &mY);
        const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
        int mWidth = mode->width;
        int mHeight = mode->height;

        int overlap =
            std::max(0, std::min(wPosX + wWidth, mX + mWidth) - std::max(wPosX, mX)) *
            std::max(0, std::min(wPosY + wHeight, mY + mHeight) - std::max(wPosY, mY));

        if (overlap > bestOverlap)
        {
            bestOverlap = overlap;
            bestMonitor = monitors[i];
        }
    }

    return bestMonitor;
}

void MacOsWindow::Shutdown()
{
    glfwDestroyWindow(m_Window);
}

}
