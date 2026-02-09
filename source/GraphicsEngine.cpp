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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

void GraphicsEngine::Render()
{
	while (!render_queue.empty()) {
		RenderItem item = render_queue.front();
		render_queue.pop();

		if (!item.model) {
			continue;
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
		shader->uniformmat4f("modelMatrix", item.model->get_model_matrix());
		shader->uniformmat4f("normalMatrix", item.model->get_normal_matrix());

		// Render the model
		item.model->Render();
	}
}

bool GraphicsEngine::IsWindowOpen()
{
	return window && !glfwWindowShouldClose(window);
}

void GraphicsEngine::UpdateWindow(glm::vec4 surface_color)
{
	// Swap front and back buffers
	glfwSwapBuffers(window);

	// Poll for events
	glfwPollEvents();

	// Clear buffers for next frame
	glClearColor(surface_color.r, surface_color.g, surface_color.b, surface_color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Wait for target FPS
	std::this_thread::sleep_for(std::chrono::milliseconds(1000 / properties.frames_per_second));
}
