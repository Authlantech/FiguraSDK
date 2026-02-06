#include <Figura/GraphicsEngine.h>
#include <chrono>

using namespace fgr;

GraphicsEngine::GraphicsEngine(WindowProperties properties) : properties(properties)
{
	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return;
	}

	// Configure OpenGL context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, properties.OpenGLContextVersionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, properties.OpenGLContextVersionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window
	window = glfwCreateWindow(
		properties.width, 
		properties.height, 
		properties.tittle, 
		nullptr, 
		nullptr
	);

	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		window = nullptr;
		return;
	}

	// Set up default OpenGL state
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	// Load default shader from hardcoded source in Shader.h
	default_shader = std::make_shared<Shader>();
	default_shader->LoadFromBuffer(model_vs, model_fs);
}

GraphicsEngine::~GraphicsEngine()
{
	if (window) {
		glfwDestroyWindow(window);
	}
	glfwTerminate();
}

void GraphicsEngine::SetDefaultShader(std::shared_ptr<Shader> shader)
{
	default_shader = shader;
}

void GraphicsEngine::SetDefaultCamera(std::shared_ptr<Camera> camera)
{
	default_camera = camera;
}

void GraphicsEngine::AppendRenderQueue(RenderItem item)
{
	render_queue.push(item);
}

void GraphicsEngine::ClearRenderQueue()
{
	// Clear the queue by swapping with an empty queue
	std::queue<RenderItem> empty;
	std::swap(render_queue, empty);
}

void GraphicsEngine::LoadModel(std::shared_ptr<Model> model, std::string file)
{
	// Launch LoadModelData in a background thread using std::async
	// The result is stored in the model's is_loaded future
	model->is_loaded = std::async(std::launch::async, LoadModelData, file);
}

void GraphicsEngine::Render()
{
	while (!render_queue.empty()) {
		RenderItem item = render_queue.front();
		render_queue.pop();

		if (!item.model) {
			continue;
		}

		// Check if model has pending data to load from async operation
		if (item.model->is_loaded.valid()) {
			// Check if the future is ready without blocking
			auto status = item.model->is_loaded.wait_for(std::chrono::seconds(0));
			if (status == std::future_status::ready) {
				// Load the model data on the main thread (OpenGL context)
				model_data data = item.model->is_loaded.get();
				item.model->LoadFromData(data);
				// Future is now invalid (reset) after get(), preventing multiple loads
			}
		}

		// Determine which shader to use
		std::shared_ptr<Shader> shader = item.shader ? item.shader : default_shader;
		if (!shader) {
			continue; // No shader available, skip this item
		}

		// Determine which camera to use
		std::shared_ptr<Camera> camera = item.camera ? item.camera : default_camera;
		if (!camera) {
			continue; // No camera available, skip this item
		}

		// Use shader
		shader->use();

		// Set camera uniforms directly (view and projection matrices)
		glm::vec3 camPos = camera->get_position();
		shader->uniformvec3("viewPos", camPos.x, camPos.y, camPos.z);
		shader->uniformmat4f("projectionMatrix", camera->get_projectionMatrix());
		shader->uniformmat4f("viewMatrix", camera->get_viewMatrix());

		// Set model uniforms
		shader->uniformmat4f("modelMatrix", item.model->get_modelMatrix());
		shader->uniformmat4f("normalMatrix", item.model->get_normalMatrix());

		// Render the model
		item.model->Render();
	}
}

bool GraphicsEngine::IsWindowOpen()
{
	return window && !glfwWindowShouldClose(window);
}

void GraphicsEngine::UpdateWindow()
{
	// Swap front and back buffers
	glfwSwapBuffers(window);

	// Poll for events
	glfwPollEvents();

	// Clear buffers for next frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
