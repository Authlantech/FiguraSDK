#pragma once

#include <Figura/Scene.h>
#include <GLFW/glfw3.h>

namespace fgr {

    class GraphicsEngine {
        GLFWwindow* window = nullptr;
        std::unordered_map<std::string, SCENE> scenes;
        public:

        void     initWindow(int width,int height,const char* title);
        void     deleteWindow();

        SCENE   createScene(std::string name);
        SCENE   getScene(std::string name);
        void    deleteScene(std::string name);

        bool     isWindowOpen();
        void     updateWindow();
    };
}