#include <Figura/GraphicsEngine.h>
#include <Figura/Light.h>

#include <iostream>
#include <thread>
#include <chrono>

int main()
{
	// Window configuration
	fgr::WindowProperties props;
	props.width = 800;
	props.height = 600;
	props.tittle = "FiguraSDK Example";
	props.OpenGLContextVersionMajor = 4;
	props.OpenGLContextVersionMinor = 6;

	// Initialize the graphics engine
	fgr::GraphicsEngine engine(props);

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

	// Initialize lighting system
	fgr::DirectionalLight::begin_directional_lights();

	// Create a perspective camera
	auto camera = std::make_shared<fgr::Camera>();
	camera->configure_perspective(60.f, 800.f / 600.f, 0.1f, 100.f);
	camera->set_position(glm::vec3(0.f, 2.f, 10.f));
	camera->face(glm::vec3(0.f, 0.f, 0.f));
	engine.SetDefaultCamera(camera);

	// Create a directional light (sun)
	fgr::DirectionalLight sun(glm::vec3(1.f, -1.f, -0.5f), glm::vec3(1.f, 1.f, 1.f));

	// Load a model
	auto model = std::make_shared<fgr::Model>();
	const char* model_path = "../assets/Meshy_AI_Fortress_Cannon_Cart_0206220959_texture.obj";
	engine.LoadModel(model, model_path);
	model->set_position(glm::vec3(0.f, 0.f, 0.f));

	// Main render loop
	while (engine.IsWindowOpen())
	{
		// Rotate the model slowly
		model->rotate(glm::vec3(0.f, 1.f, 0.f), 1.f);

		// Add model to render queue
		fgr::RenderItem item;
		item.model = model;
		engine.AppendRenderQueue(item);

		// Render all queued items
		engine.Render();

		// Update window (swap buffers, poll events)
		engine.UpdateWindow();

		// Cap frame rate to ~60 FPS
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	return 0;
}
