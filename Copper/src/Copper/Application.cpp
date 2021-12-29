#include "cprpch.h"
#include "Application.h"
#include "Event/ApplicationEvent.h"
#include "Log.h"
#include "ImGuiHandler.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"

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

    RenderCommand::Init();

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.2f, 0.1f, 0.8f, 1.0f,
         0.5f, -0.5f, 0.0f, 0.7f, 0.4f, 0.3f, 1.0f,
         0.0f,  0.5f, 0.0f, 0.4f, 0.9f, 0.5f, 1.0f
    };

    std::shared_ptr<VertexBuffer> vertexBuffer(VertexBuffer::Create(vertices, 21));

    BufferLayout layout = {
        { ShaderData::Type::Float3, "aPos" },
        { ShaderData::Type::Float4, "aColor" }
    };

    vertexBuffer->SetLayout(layout);

    uint32_t indices[] = { 0, 1, 2 };
    std::shared_ptr<IndexBuffer> indexBuffer(IndexBuffer::Create(indices, 3));

    m_VertexArray = std::shared_ptr<VertexArray>(VertexArray::Create());

    m_VertexArray->AddVertexBuffer(vertexBuffer);
    m_VertexArray->SetIndexBuffer(indexBuffer);

    std::string vertexShaderSrc = "#version 410 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec4 aColor;\n"
        "out vec4 vColor;\n"
        "void main() {\n"
        "gl_Position = vec4(aPos, 1.0f);\n"
        "vColor = aColor;\n"
        "}";
    std::string fragmetShaderSrc = "#version 410 core\n"
        "out vec4 FragColor;\n"
        "in vec4 vColor;\n"
        "void main() { FragColor = vColor; }";

    m_Shader = std::unique_ptr<Shader>(Shader::Create(vertexShaderSrc, fragmetShaderSrc));

    s_Instance = this;
}

int Application::Run()
{
    while (m_Running)
    {
        RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
        RenderCommand::Clear();

        Renderer::BeginScene();

        m_Shader->Bind();
        Renderer::Submit(m_VertexArray);

        Renderer::EndScene();

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
