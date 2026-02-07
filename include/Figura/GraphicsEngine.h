#pragma once

#include <iostream>
#include <queue>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Figura/Model.h>
#include <Figura/Camera.h>

namespace fgr {

    struct WindowProperties
    {
        int width = 0; 
        int height = 0; 
        const char* tittle = 0;
        int frames_per_second = 30;
        GLint OpenGLContextVersionMajor = 4; 
        GLint OpenGLContextVersionMinor = 6;
    };

    struct RenderItem
    {
        std::shared_ptr<Shader> shader = nullptr; 
        std::shared_ptr<Model>  model = nullptr; 
    };

    class GraphicsEngine
    {
    protected : 
        GLFWwindow* window = nullptr;
        WindowProperties properties = {};

        std::queue<RenderItem> render_queue;

        std::shared_ptr<Shader> default_shader; 
        std::shared_ptr<Camera> default_camera;
    public:
        GraphicsEngine(WindowProperties properties); 
        ~GraphicsEngine(); 

        void ConfigureDefaultShader(std::shared_ptr<Shader> shader); 
        void ConfigureCamera(std::shared_ptr<Camera> camera); 
        void GetCameraMovement();

        void AppendRenderQueue(RenderItem item); 
        void ClearRenderQueue();
        void Render();

        std::shared_ptr<Model> LoadModelAsync(std::string file);
        std::shared_ptr<Model> LoadModel(std::string file);
        bool IsWindowOpen();
        void UpdateWindow(); 
    };
}