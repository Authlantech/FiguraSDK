#include <Figura/GraphicsEngine.h>
#include <set>

using namespace fgr;

ModelPtr Scene::createModel(std::string name) {
    ModelPtr model = std::make_shared<Model>();
    models[name] = model;
    return model;
}

void Scene::createModel(std::string name, ModelPtr source_model) {
    if (source_model != nullptr)
    models[name] = source_model;
}

void Scene::createModel(std::string name, std::string path) {
    ModelPtr model = std::make_shared<Model>();
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


ShaderPtr Scene::createShader(std::string name) {
    ShaderPtr shader = std::make_shared<Shader>();
    shaders[name] = shader;
    return shader;
}

void Scene::createShader(std::string name, ShaderPtr source_shader) {
    if (source_shader != nullptr)
    shaders[name] = source_shader;
}

void Scene::useShader(std::string name) {
    ShaderPtr shader = getShader(name);
    if (shader != nullptr) {
        currentShader = shader;
        currentShader->use();
    }
}

CameraPtr Scene::createCamera(std::string name) {
    CameraPtr camera = std::make_shared<Camera>();
    cameras[name] = camera;
    return camera;
}

void Scene::createCamera(std::string name, CameraPtr source_camera) {
    if (source_camera != nullptr)
        cameras[name] = source_camera;
}

void Scene::useCamera(std::string name) {
    CameraPtr camera = getCamera(name);
    if (camera != nullptr) {
        currentCamera = camera;
        currentCamera->use(currentShader);
    }
}

DirectionalLightPtr Scene::createDirectionalLight(std::string name,glm::vec3 direction,glm::vec3 color) {
    DirectionalLightPtr light = std::make_shared<DirectionalLight>(direction,color);
    directional_lights[name] = light;
    return light;
}

PointLightPtr Scene::createPointLight(std::string name,glm::vec3 position,glm::vec3 color) {
    PointLightPtr light = std::make_shared<PointLight>(position,color);
    point_lights[name] = light;
    return light;
}

SpotLightPtr Scene::createSpotLight(std::string name, glm::vec3 position, glm::vec3 color, glm::vec3 orientation, float angle) {
    SpotLightPtr light = std::make_shared<SpotLight>(position, color, orientation, angle);
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

ModelPtr Scene::getModel(std::string name) {
    ModelPtr model = nullptr;
    try {
        model = models.at(name);
    }

    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return nullptr;
    }

    return model;
}

ShaderPtr Scene::getShader(std::string name) {
    ShaderPtr shader = nullptr;
    try {
        shader = shaders.at(name);
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return nullptr;
    }
    return shader;
}

CameraPtr Scene::getCamera(std::string name) {
    CameraPtr camera =  nullptr;
    try {
        camera = cameras.at(name);
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return nullptr;
    }
    return camera;
}

DirectionalLightPtr Scene::getDirectionalLight(std::string name) {
    DirectionalLightPtr directional_light = nullptr;
    try {
        directional_light = directional_lights.at(name);
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return nullptr;
    }
    return directional_light;
}

PointLightPtr Scene::getPointLight(std::string name) {
    PointLightPtr point_light = nullptr;
    try {
        point_light = point_lights.at(name);
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return nullptr;
    }
    return point_light;
}

SpotLightPtr Scene::getSpotLight(std::string name) {
    SpotLightPtr spot_light = nullptr;
    try {
        spot_light = spot_lights.at(name);
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return nullptr;
    }
    return spot_light;
}

void Scene::RenderScene() {
    if (currentShader == nullptr)
        return;

    for (const auto model : models) {
        if (load_status.find(model.first) != load_status.end()) {
            if (load_status[model.first].wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                model.second->LoadFromData(loaded_data[model.first]);
                load_status.erase(model.first);
                loaded_data.erase(model.first);
            }
        }

        model.second->Render(currentShader);
    }
}
