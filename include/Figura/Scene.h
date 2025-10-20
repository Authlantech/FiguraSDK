#pragma once

#include <unordered_map>
#include <string>

#include <Figura/Camera.h>
#include <Figura/Model.h>
#include <Figura/Light.h>
#include <Figura/Shader.h>
#include <future>

namespace fgr {

    typedef std::shared_ptr<fgr::Model>             ModelPtr;
    typedef std::shared_ptr<fgr::Shader>            ShaderPtr;
    typedef std::shared_ptr<fgr::Camera>            CameraPtr;
    typedef std::shared_ptr<fgr::DirectionalLight>  DirectionalLightPtr;
    typedef std::shared_ptr<fgr::PointLight>        PointLightPtr;
    typedef std::shared_ptr<fgr::SpotLight>         SpotLightPtr;

    class Scene {
        std::unordered_map<std::string, ModelPtr>             models;
        std::unordered_map<std::string, ShaderPtr>            shaders;
        std::unordered_map<std::string, CameraPtr>            cameras;
        std::unordered_map<std::string, DirectionalLightPtr>  directional_lights;
        std::unordered_map<std::string, PointLightPtr>        point_lights;
        std::unordered_map<std::string, SpotLightPtr>         spot_lights;

        ShaderPtr currentShader;
        CameraPtr currentCamera;
    public :
        Scene() = default;
        ~Scene() = default;

        ModelPtr            createModel(std::string name);
        void                createModel(std::string name, ModelPtr source_model);
        void                createModel(std::string name,std::string path);
    private :
        std::unordered_map<std::string,std::future<void> > load_status;
        std::unordered_map<std::string,Model::DATA >       loaded_data;
    public :
        ShaderPtr           createShader(std::string name);
        void                createShader(std::string name, ShaderPtr source_shader);
        void                useShader(std::string name);

        CameraPtr           createCamera(std::string name);
        void                createCamera(std::string name, CameraPtr source_camera);
        void                useCamera(std::string name);

        DirectionalLightPtr createDirectionalLight(std::string name,glm::vec3 direction, glm::vec3 color);
        PointLightPtr       createPointLight(std::string name,glm::vec3 position,glm::vec3 color);
        SpotLightPtr        createSpotLight(std::string name,glm::vec3 position,glm::vec3 color,glm::vec3 orientation,float angle);

        void deleteModel(std::string name);
        void deleteShader(std::string name);
        void deleteCamera(std::string name);
        void deleteDirectionalLight(std::string name);
        void deletePointLight(std::string name);
        void deleteSpotLight(std::string name);

        ModelPtr            getModel(std::string name);
        ShaderPtr           getShader(std::string name);
        CameraPtr           getCamera(std::string name);
        DirectionalLightPtr getDirectionalLight(std::string name);
        PointLightPtr       getPointLight(std::string name);
        SpotLightPtr        getSpotLight(std::string name);

        std::vector<std::string> get_all_model_names();
        std::vector<std::string> get_all_shader_names();
        std::vector<std::string> get_all_cameras_names();
        std::vector<std::string> get_all_directional_light_names();
        std::vector<std::string> get_all_point_light_names();
        std::vector<std::string> get_all_spot_light_names();

        void RenderScene();
    };

    typedef std::shared_ptr<Scene> ScenePtr;

}