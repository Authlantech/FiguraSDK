#include <Figura/Figura.h>

#include <iostream> 

int window_width = 800; 
int window_height = 800; 
const char window_tittle[] = "example program";

int main()
{
	//Init Figura : 
	fgr::graphic_engine.init_engine("glfw example",800,800);

	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_RENDERER) << std::endl; 

	//Create camera : 	
	auto camera = fgr::graphic_engine.create_perspective_camera("default camera", 60.f , window_width / window_height, 0.01,1000);	
	camera->set_position({ 0,0,5 });

	//Load a model
	const char* model_path = "../../../../Documents/3D Models/Fortress_Cannon_Cart_1012161648_texture_obj/Fortress_Cannon_Cart_1012161648_texture_obj/Fortress_Cannon_Cart_1012161648_texture.obj";
	auto model = fgr::graphic_engine.create_and_load_model("model1", model_path);

	//Create a light source : 
	fgr::graphic_engine.create_directional_light("sun", { 1,-1,0 }, { 1,1,1 });

	//Window loop 
	while (fgr::graphic_engine.window_is_open())
	{					
		model->rotate({ 0,1,0 }, 4.f);

		camera->use();
		model->Render();

		/*
		RENDERING SCENE WITH OBJECT LABELS, DOES THE SAME THING AS ABOVE 
		fgr::graphic_engine.get_camera("default camera")->use();
		fgr::graphic_engine.get_model("model1")->Render();
		*/

		fgr::graphic_engine.update_window();

		_sleep(1000.f / 25.f);
	}

}