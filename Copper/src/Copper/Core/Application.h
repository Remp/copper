#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "Copper/Event/ApplicationEvent.h"
#include "ImGuiHandler.h"

namespace Copper
{

class COPPER_API Application
{
public:
    Application();
    virtual ~Application() = default;
    
    virtual int Run();

    void OnEvent(Event& event);

    void PushLayer(Layer* layer);
    void PushOverlaye(Layer* overlay);

    inline const Window& GetWindow() const { return *m_Window; }
    
    inline static const Application& Get() { return *s_Instance; }
private:
    bool OnWindowClose(WindowCloseEvent& e);

    std::unique_ptr<Window> m_Window;
    std::unique_ptr<ImGuiHandler> m_ImGuiHandler;
    float m_LastFrameTime = 0.0f;
    bool m_Running = true;
    LayerStack m_LayerStack;

    static Application* s_Instance;
};

// to be implemented by client
Application* CreateApplication();

}
