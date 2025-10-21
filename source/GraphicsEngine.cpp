#include <Figura/GraphicsEngine.h>

using namespace fgr;


void GraphicsEngine::initWindow(int width,int height,const char* title) {

    // Init GLFW
    int res = glfwInit();
    if (res == GLFW_FALSE)
    {
        printf("glfw could not be initilaized!\n");
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window

    window = glfwCreateWindow(width, height, title, 0, 0);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Init glad
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    //Begin Light Buffers
    fgr::DirectionalLight::begin_directional_lights();
    fgr::PointLight::begin_point_lights();
    fgr::SpotLight::begin_spot_light();

    //Enable Depth Testing and Face Culling :
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void GraphicsEngine::deleteWindow() {

}

SCENE GraphicsEngine::createScene(std::string name) {
    SCENE scene = std::make_shared<Scene>();
    scenes[name] = scene;

    auto modelShader = scene->createShader("figura_default_model_shader");
    modelShader->LoadFromBuffer(model_vs,model_fs);

    auto normalShader = scene->createShader("figura_default_normal_shader");
    normalShader->LoadFromBuffer(normal_vs,normal_fs,normal_gs);

    auto meshShader = scene->createShader("figura_default_mesh_shader");
    meshShader->LoadFromBuffer(mesh_vs,mesh_fs,mesh_gs);

    scene->useShader("figura_default_model_shader");

    return scene;
}

SCENE GraphicsEngine::getScene(const std::string name) {
    SCENE scene = nullptr;
    try {
        scene = scenes.at(name);
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return nullptr;
    }
    return scene;
}

void GraphicsEngine::deleteScene(const std::string name) {

}

bool GraphicsEngine::isWindowOpen() {
    return !glfwWindowShouldClose(window);
}

void GraphicsEngine::updateWindow() {
    glfwPollEvents();
    glfwSwapBuffers(window);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0.4, 1, 0);
}




