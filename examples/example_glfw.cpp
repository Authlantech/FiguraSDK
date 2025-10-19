#include <Figura/GraphicsEngine.h>

#include <iostream> 


int window_width = 800; 
int window_height = 800; 
const char window_tittle[] = "example program";

int main()
{
	//Init Figura : 
	fgr::GraphicsEngine engine;
	engine.initWindow(window_width,window_height,window_tittle);

	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_RENDERER) << std::endl; 

	//Create a scene :
	auto scene = engine.createScene("default");

	//Create and load shaders :
	auto model_shader = scene->createShader("model shader");
	model_shader->load_from_buffer(fgr::model_vs,fgr::model_fs);

	auto mesh_shader = scene->createShader("mesh shader");
	mesh_shader->load_from_buffer(fgr::mesh_vs,fgr::mesh_fs,fgr::mesh_gs);

	//Create camera :
	auto camera = scene->createCamera("default camera");
	camera->configure_perspective(60.f, window_width / window_height,0.1f,100.f);

	//Load a model
	const char* model_path = "../../../../Documents/3D Models/Fortress_Cannon_Cart_1012161648_texture_obj/Fortress_Cannon_Cart_1012161648_texture_obj/Fortress_Cannon_Cart_1012161648_texture.obj";
	auto model = scene->createModel("model1");
	model->LoadFromData(fgr::Model::LoadModelData(model_path));
	model->set_position({0,0,-10});

	//Create a light source : 
	scene->createDirectionalLight("sun",{ 1,-1,0 }, { 1,1,1 });

	//Window loop 
	while (engine.isWindowOpen())
	{					
		model->rotate({ 0,1,0 }, 4.f);

		scene->useShader("model shader");
		scene->useCamera("default camera");
		scene->RenderScene();

		engine.updateWindow();

		_sleep(1000.f / 25.f);
	}

}

