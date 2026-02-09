#include <Figura/GraphicsEngine.h>

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

	// Create a default shader
	auto shader = std::make_shared<fgr::Shader>();
	shader->LoadFromFile("../../shaders/default/default.vert", "../../shaders/default/default.frag");
	engine.ConfigureDefaultShader(shader);

	// Create a default perspective camera
	auto camera = std::make_shared<fgr::Camera>();
	camera->configure_perspective(60.f, 800.f / 600.f, 0.1f, 100.f);
	camera->set_position(glm::vec3(0.f, 2.f, 10.f));
	camera->face(glm::vec3(0.f, 0.f, 0.f));
	engine.ConfigureCamera(camera);

	// Load a model
	auto model = engine.LoadModelAsync("../assets/Meshy_AI_Fortress_Cannon_Cart_0206220959_texture.obj");
	model->set_position(glm::vec3(0.f, 0.f, 0.f));

	// Main render loop
	while (engine.IsWindowOpen())
	{
		// Rotate the model slowly
		model->rotate(glm::vec3(0.f, 1.f, 0.f), 1.f);

		// Get camera movement
		engine.GetCameraMovement();

		// Add model to render queue
		fgr::RenderItem item;
		item.model = model;
		engine.AppendRenderQueue(item);

		// Render all queued items
		engine.Render();

		// Update window (swap buffers, poll events)
		engine.UpdateWindow();
	}

	return 0;
}
