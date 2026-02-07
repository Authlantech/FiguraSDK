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

}

GraphicsEngine::~GraphicsEngine()
{
	if (window) {
		glfwDestroyWindow(window);
	}
	glfwTerminate();
}

void GraphicsEngine::ConfigureDefaultShader(std::shared_ptr<Shader> shader)
{
	default_shader = shader;
}

void GraphicsEngine::ConfigureCamera(std::shared_ptr<Camera> camera)
{
	default_camera = camera;
}

void GraphicsEngine::GetCameraMovement()
{
	float deltaTime = 1.f / properties.frames_per_second;

	if (!default_camera || !window) {
		return;
	}

	float speed = 5.0f * deltaTime;
	glm::vec3 position = default_camera->get_position();
	glm::vec3 orientation = default_camera->get_oreintation();
	glm::vec3 right = glm::normalize(glm::cross(orientation, glm::vec3(0.f, 1.f, 0.f)));

	// WASD movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += orientation * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= orientation * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= right * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += right * speed;
	}

	// Up/Down movement
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		position += glm::vec3(0.f, 1.f, 0.f) * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		position -= glm::vec3(0.f, 1.f, 0.f) * speed;
	}

	default_camera->set_position(position);

	// Mouse look (when right mouse button is held)
	static bool firstMouse = true;
	static double lastX = 0.0, lastY = 0.0;
	static float yaw = -90.0f, pitch = 0.0f;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		if (firstMouse) {
			lastX = mouseX;
			lastY = mouseY;
			firstMouse = false;
		}

		float sensitivity = 0.1f;
		float xOffset = static_cast<float>(mouseX - lastX) * sensitivity;
		float yOffset = static_cast<float>(lastY - mouseY) * sensitivity;

		lastX = mouseX;
		lastY = mouseY;

		yaw += xOffset;
		pitch += yOffset;

		// Clamp pitch to avoid flipping
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		glm::vec3 newOrientation;
		newOrientation.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		newOrientation.y = sin(glm::radians(pitch));
		newOrientation.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		
		default_camera->face(default_camera->get_position() + glm::normalize(newOrientation));
	}
	else {
		firstMouse = true;
	}
}

void GraphicsEngine::AppendRenderQueue(RenderItem item)
{
	render_queue.push(item);
}

void GraphicsEngine::ClearRenderQueue()
{
	std::queue<RenderItem> empty;
	std::swap(render_queue, empty);
}

std::shared_ptr<Model> GraphicsEngine::LoadModelAsync(std::string file)
{
	std::shared_ptr<Model> new_model = std::make_shared<Model>();
	new_model->is_loaded = std::async(std::launch::async, &Model::LoadModelData, file);
	return new_model;
}

std::shared_ptr<Model> GraphicsEngine::LoadModel(std::string file)
{
	std::shared_ptr<Model> new_model = std::make_shared<Model>();
	model_data dat = Model::LoadModelData(file);
	new_model->LoadFromData(dat);
	return new_model;
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

		if(!default_camera) {
			continue;
		}

		// Use shader
		shader->use();

		// Set camera uniforms directly (view and projection matrices)
		glm::vec3 camPos = default_camera->get_position();
		shader->uniformvec3("viewPos", camPos.x, camPos.y, camPos.z);
		shader->uniformmat4f("projectionMatrix", default_camera->get_projectionMatrix());
		shader->uniformmat4f("viewMatrix", default_camera->get_viewMatrix());

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

	//Wait for target FPS
	std::this_thread::sleep_for(std::chrono::milliseconds(1000 / properties.frames_per_second));
}
