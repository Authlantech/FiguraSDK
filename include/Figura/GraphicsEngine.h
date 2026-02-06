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
        std::shared_ptr<Camera> camera = nullptr;
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

        void SetDefaultShader(std::shared_ptr<Shader> shader); 
        void SetDefaultCamera(std::shared_ptr<Camera> camera); 

        void AppendRenderQueue(RenderItem item); 
        void ClearRenderQueue();
        void Render();

        void LoadModel(std::shared_ptr<Model> model,std::string file);
        bool IsWindowOpen();
        void UpdateWindow(); 
    };
}