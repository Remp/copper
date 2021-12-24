#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "Event/ApplicationEvent.h"
#include "ImGuiHandler.h"
#include "Copper/Renderer/Shader.h"
#include "Copper/Renderer/Buffer.h"
#include "Copper/Renderer/VertexArray.h"

namespace Copper
{

class COPPER_API Application
{
public:
    Application();
    virtual ~Application() = default;
    
    virtual int Run();

    void OnEvent(Event& event);
    bool OnWindowClose(WindowCloseEvent& event);

    void PushLayer(Layer* layer);
    void PushOverlaye(Layer* overlay);

    inline const Window& GetWindow() const { return *m_Window; }
    
    inline static const Application& Get() { return *s_Instance; }
private:
    std::unique_ptr<Window> m_Window;
    std::unique_ptr<ImGuiHandler> m_ImGuiHandler;
    bool m_Running = true;
    LayerStack m_LayerStack;

    std::unique_ptr<Shader> m_Shader;
    std::shared_ptr<VertexArray> m_VertexArray;

    static Application* s_Instance;
};

// to be implemented by client
Application* CreateApplication();

}
