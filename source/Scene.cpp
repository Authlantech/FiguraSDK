#include <Figura/GraphicsEngine.h>

using namespace fgr;

MODEL Scene::createModel(std::string name) {
    MODEL model = std::make_shared<Model>();
    models[name] = model;
    return model;
}

void Scene::createModel(std::string name, std::string path) {
    MODEL model = std::make_shared<Model>();
    models[name] = model;

    std::promise<void> promise;
    load_status.insert({name, promise.get_future()});

    auto load_process = [this](std::string name,std::string path,std::promise<void>&& load_alert) ->void {
        this->loaded_data[name] = Model::LoadModelData(path);
        load_alert.set_value();
    };

    std::thread load_thread(load_process,name,path, std::move(promise));
    load_thread.detach();
}

void Scene::RenderModel(std::string name) {
    auto model = getModel(name);
    if (model != nullptr) {
        if (load_status.find(name) != load_status.end()) {
            if (load_status[name].wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                model->LoadFromData(loaded_data[name]);
                load_status.erase(name);
                loaded_data.erase(name);
            }
        }

        if (currentShader != nullptr)
            currentShader->use();
        if (currentCamera != nullptr)
            currentCamera->use(currentShader);

        model->Render(currentShader);
    }
}

SHADER Scene::createShader(std::string name) {
    SHADER shader = std::make_shared<Shader>();
    shaders[name] = shader;
    return shader;
}

void Scene::useShader(std::string name) {
    SHADER shader = getShader(name);
    if (shader != nullptr) {
        currentShader = shader;
        currentShader->use();
    }
}

CAMERA Scene::createCamera(std::string name) {
    CAMERA camera = std::make_shared<Camera>();
    cameras[name] = camera;
    return camera;
}

void Scene::useCamera(std::string name) {
    CAMERA camera = getCamera(name);
    if (camera != nullptr) {
        currentCamera = camera;
        currentCamera->use(currentShader);
    }
}

DIRECTIONAL_LIGHT Scene::createDirectionalLight(std::string name,glm::vec3 direction,glm::vec3 color) {
    DIRECTIONAL_LIGHT light = std::make_shared<DirectionalLight>(direction,color);
    directional_lights[name] = light;
    return light;
}

POINT_LIGHT Scene::createPointLight(std::string name,glm::vec3 position,glm::vec3 color) {
    POINT_LIGHT light = std::make_shared<PointLight>(position,color);
    point_lights[name] = light;
    return light;
}

SPOT_LIGHT Scene::createSpotLight(std::string name, glm::vec3 position, glm::vec3 color, glm::vec3 orientation, float angle) {
    SPOT_LIGHT light = std::make_shared<SpotLight>(position, color, orientation, angle);
    spot_lights[name] = light;
    return light;
}

void Scene::deleteModel(std::string name) {
    models.erase(name);
}

void Scene::deleteShader(std::string name) {
    shaders.erase(name);
}

void Scene::deleteCamera(std::string name) {
    cameras.erase(name);
}

void Scene::deleteDirectionalLight(std::string name) {
    directional_lights.erase(name);
}

void Scene::deletePointLight(std::string name) {
    point_lights.erase(name);
}

void Scene::deleteSpotLight(std::string name) {
    spot_lights.erase(name);
}

MODEL Scene::getModel(std::string name) {
    MODEL model = nullptr;
    try {
        model = models.at(name);
    }

    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return nullptr;
    }

    return model;
}

SHADER Scene::getShader(std::string name) {
    SHADER shader = nullptr;
    try {
        shader = shaders.at(name);
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return nullptr;
    }
    return shader;
}

CAMERA Scene::getCamera(std::string name) {
    CAMERA camera =  nullptr;
    try {
        camera = cameras.at(name);
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return nullptr;
    }
    return camera;
}

DIRECTIONAL_LIGHT Scene::getDirectionalLight(std::string name) {
    DIRECTIONAL_LIGHT directional_light = nullptr;
    try {
        directional_light = directional_lights.at(name);
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return nullptr;
    }
    return directional_light;
}

POINT_LIGHT Scene::getPointLight(std::string name) {
    POINT_LIGHT point_light = nullptr;
    try {
        point_light = point_lights.at(name);
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return nullptr;
    }
    return point_light;
}

SPOT_LIGHT Scene::getSpotLight(std::string name) {
    SPOT_LIGHT spot_light = nullptr;
    try {
        spot_light = spot_lights.at(name);
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return nullptr;
    }
    return spot_light;
}

std::vector<std::string> Scene::get_all_model_names() {
    std::vector<std::string> names;
    for (auto m : models) {
        names.push_back(m.first);
    }
    return names;
}

std::vector<std::string> Scene::get_all_shader_names() {
    std::vector<std::string> names;
    for (auto m : shaders) {
        names.push_back(m.first);
    }
    return names;
}

std::vector<std::string> Scene::get_all_cameras_names() {
    std::vector<std::string> names;
    for (auto m : cameras) {
        names.push_back(m.first);
    }
    return names;
}

std::vector<std::string> Scene::get_all_directional_light_names() {
    std::vector<std::string> names;
    for (auto m : directional_lights) {
        names.push_back(m.first);
    }
    return names;
}

std::vector<std::string> Scene::get_all_point_light_names() {
    std::vector<std::string> names;
    for (auto m : point_lights) {
        names.push_back(m.first);
    }
    return names;
}

std::vector<std::string> Scene::get_all_spot_light_names() {
    std::vector<std::string> names;
    for (auto m : spot_lights) {
        names.push_back(m.first);
    }
    return names;
}

