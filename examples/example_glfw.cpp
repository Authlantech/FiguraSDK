#include <Figura/Figura.h>
#include <GLFW/glfw3.h>

#include <iostream> 
#include <vector>

int window_width = 800; 
int window_height = 800; 
const char window_tittle[] = "example program";

int main()
{
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

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, window_tittle, 0, 0);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);	

	//Init Figura : 
	fgr::graphic_engine.init_engine((GLADloadproc)glfwGetProcAddress);		

	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_RENDERER) << std::endl; 

	//Create camera : 	
	auto camera = fgr::graphic_engine.create_perspective_camera("default camera", 60.f , window_width / window_height, 0.01,1000);	
	camera->set_position({ 0,0,5 });

	//Load a model
	const char* model_path = "../../../../Documents/3D Models/Fortress_Cannon_Cart_1012161648_texture_obj/Fortress_Cannon_Cart_1012161648_texture_obj/Fortress_Cannon_Cart_1012161648_texture.obj";
	auto model = fgr::graphic_engine.create_model("model1", model_path);		

	//Create a light source : 
	fgr::graphic_engine.create_directional_light("sun", { 1,-1,0 }, { 1,1,1 });

	//Window loop 
	while (!glfwWindowShouldClose(window))
	{					
		model->rotate({ 0,1,0 }, 1.f);		

		camera->use();
		model->Render();

		/*
		RENDERING SCENE WITH OBJECT LABELS, DOES THE SAME THING AS ABOVE 
		fgr::graphic_engine.get_camera("default camera")->use();
		fgr::graphic_engine.get_model("model1")->Render();
		*/

		glfwPollEvents();
		glfwSwapBuffers(window); 		

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0, 0, 1, 0);

		_sleep(1000.f / 25.f);
	}

}