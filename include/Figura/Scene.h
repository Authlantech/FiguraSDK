#pragma once

#include <unordered_map>
#include <string>

#include <Figura/Camera.h>
#include <Figura/Model.h>
#include <Figura/Light.h>
#include <Figura/Shader.h>
#include <future>

namespace fgr {

    typedef std::shared_ptr<fgr::Model>             MODEL;
    typedef std::shared_ptr<fgr::Shader>            SHADER;
    typedef std::shared_ptr<fgr::Camera>            CAMERA;
    typedef std::shared_ptr<fgr::DirectionalLight>  DIRECTIONAL_LIGHT;
    typedef std::shared_ptr<fgr::PointLight>        POINT_LIGHT;
    typedef std::shared_ptr<fgr::SpotLight>         SPOT_LIGHT;

    class Scene {
        std::unordered_map<std::string, MODEL>              models;
        std::unordered_map<std::string, SHADER>             shaders;
        std::unordered_map<std::string, CAMERA>             cameras;
        std::unordered_map<std::string, DIRECTIONAL_LIGHT>  directional_lights;
        std::unordered_map<std::string, POINT_LIGHT>        point_lights;
        std::unordered_map<std::string, SPOT_LIGHT>         spot_lights;

        SHADER currentShader;
        CAMERA currentCamera;
    public :
        Scene() = default;
        ~Scene() = default;

        MODEL   createModel(std::string name);
        void    createModel(std::string name,std::string path);
        void    RenderModel(std::string name);
    private :
        std::unordered_map<std::string,std::future<void> > load_status;
        std::unordered_map<std::string,MODEL_DATA >       loaded_data;
    public :
        SHADER  createShader(std::string name);
        void    useShader(std::string name);

        CAMERA  createCamera(std::string name);
        void    useCamera(std::string name);

        DIRECTIONAL_LIGHT   createDirectionalLight(std::string name,glm::vec3 direction, glm::vec3 color);
        POINT_LIGHT         createPointLight(std::string name,glm::vec3 position,glm::vec3 color);
        SPOT_LIGHT          createSpotLight(std::string name,glm::vec3 position,glm::vec3 color,glm::vec3 orientation,float angle);

        void deleteModel(std::string name);
        void deleteShader(std::string name);
        void deleteCamera(std::string name);
        void deleteDirectionalLight(std::string name);
        void deletePointLight(std::string name);
        void deleteSpotLight(std::string name);

        MODEL             getModel(std::string name);
        SHADER            getShader(std::string name);
        CAMERA            getCamera(std::string name);
        DIRECTIONAL_LIGHT getDirectionalLight(std::string name);
        POINT_LIGHT       getPointLight(std::string name);
        SPOT_LIGHT        getSpotLight(std::string name);

        std::vector<std::string> get_all_model_names();
        std::vector<std::string> get_all_shader_names();
        std::vector<std::string> get_all_cameras_names();
        std::vector<std::string> get_all_directional_light_names();
        std::vector<std::string> get_all_point_light_names();
        std::vector<std::string> get_all_spot_light_names();
    };

    typedef std::shared_ptr<Scene> SCENE;

}