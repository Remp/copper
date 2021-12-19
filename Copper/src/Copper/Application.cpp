#include "cprpch.h"
#include "Application.h"
#include "Event/ApplicationEvent.h"
#include "Log.h"
#include "ImGuiHandler.h"
#include "glad/glad.h"

namespace Copper
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application* Application::s_Instance = nullptr;

Application::Application()
{
    CPR_ASSERT(!s_Instance, "Application is a singleton");

    m_Window = std::unique_ptr<Window>(Window::Create());
    m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
    m_ImGuiHandler = std::unique_ptr<ImGuiHandler>(ImGuiHandler::Create(*m_Window));

    glGenVertexArrays(1, &m_VertexArray);
    glBindVertexArray(m_VertexArray);

    float vertices[3 * 3] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    m_VertexBuffer = std::unique_ptr<VertexBuffer>(VertexBuffer::Create(vertices, 9));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

    uint32_t indices[3] = { 0, 1, 2 };

    m_IndexBuffer = std::unique_ptr<IndexBuffer>(IndexBuffer::Create(indices, 9));

    std::string vertexShaderSrc = "#version 410 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main() {\n"
        "gl_Position = vec4(aPos, 1.0f);\n"
        "}";
    std::string fragmetShaderSrc = "#version 410 core\n"
        "out vec4 FragColor;\n"
        "void main() { FragColor = vec4(1.0f, 0.2f, 0.6f, 1.0f); }";

    m_Shader = std::make_unique<OpenGLShader>(vertexShaderSrc, fragmetShaderSrc);

    s_Instance = this;
}

int Application::Run()
{
    while (m_Running)
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m_Shader->Bind();
        glBindVertexArray(m_VertexArray);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

        for (Layer* layer : m_LayerStack)
            layer->OnUpdate();

        m_ImGuiHandler->Begin();
        for (Layer* layer : m_LayerStack)
            layer->OnImGuiUpdate();
        m_ImGuiHandler->End();

        m_Window->OnUpdate();
    }

    return 0;
}

void Application::OnEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
    {
        (*--it)->OnEvent(event);
        if (event.IsHandled())
            break;
    }
}

bool Application::OnWindowClose(WindowCloseEvent& event)
{
    m_Running = false;

    return true;
}

void Application::PushLayer(Layer* layer)
{
    m_LayerStack.PushLayer(layer);
}

void Application::PushOverlaye(Layer* overlay)
{
    m_LayerStack.PushOverlay(overlay);
}

}
