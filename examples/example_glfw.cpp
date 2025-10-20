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

	//Create camera :
	auto camera = scene->createCamera("default camera");
	camera->configure_perspective(60.f, window_width / window_height,0.1f,100.f);
	camera->set_position({0,0,10});
	scene->useCamera("default camera");

	scene->useShader("figura_default_mesh_shader");

	//Load a model
	const char* model_path = "../../../../Documents/3D Models/Fortress_Cannon_Cart_1012161648_texture_obj/Fortress_Cannon_Cart_1012161648_texture_obj/Fortress_Cannon_Cart_1012161648_texture.obj";
	scene->createModel("model1",model_path);
	scene->getModel("model1")->set_position({0,0,-5});

	//Create a light source : 
	scene->createDirectionalLight("sun",{ 1,-1,0 }, { 1,1,1 });

	//Window loop 
	while (engine.isWindowOpen())
	{					
		scene->getModel("model1")->rotate({ 0,1,0 }, 4.f);
		scene->RenderScene();
		engine.updateWindow();

		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long>(1000.f / 60.f)));
	}

}

