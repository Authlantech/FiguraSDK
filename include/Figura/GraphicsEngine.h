#pragma once

#include <Figura/Scene.h>
#include <GLFW/glfw3.h>

namespace fgr {

    class GraphicsEngine {
        GLFWwindow* window = nullptr;
        std::unordered_map<std::string, ScenePtr> scenes;
        public:

        void     initWindow(int width,int height,const char* title);
        void     deleteWindow();

        ScenePtr createScene(const std::string name);
        void     createScene(const std::string name, ScenePtr source_scene);
        ScenePtr getScene(const std::string name);
        void     deleteScene(const std::string name);

        bool     isWindowOpen();
        void     updateWindow();
    };
}