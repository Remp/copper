#pragma once

#include "Copper/Core.h"
#include "VertexArray.h"
#include "RendererAPI.h"
#include "RenderCommand.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/glm.hpp"

namespace Copper
{

struct RenderEntity
{
    Ref<VertexArray> vertexArray;
    Ref<Shader> shader;
    Ref<Texture2D> texture;
    glm::mat4 transform = glm::mat4(1.0f);
};

class COPPER_API Renderer
{
public:
    inline static void Init() { RenderCommand::Init(); }
    static void BeginScene(const Ref<Camera>& camera);
    static void EndScene();

    static void Submit(const RenderEntity& entity);

    inline static RendererAPI::API GetAPI() { return RenderCommand::GetAPI(); }

private:
    struct SceneData
    {
        glm::mat4 ViewProjectMatrix;
    };

    static SceneData* s_SceneData;
};

}
