#include <Figura/Figura.h>
#include <GLFW/glfw3.h>

// Window attribs
struct WindowAttribs
{
	int width = 800;
	int height = 800;
	const char* title = "example";
	int fps = 144;
}w_attribs;

// Camera attribs  
fgr::Camera camera; 
struct CameraAttribs
{
	float fov = 60.f; 
	float speed = 50.f; 
	float sensitivity = 4.f;
	float near = 0.01f; 
	float far = 1000.f;
}c_attribs;

void window_resize_callback(GLFWwindow* w, int width, int height)
{
	w_attribs.width = width; 
	w_attribs.height = height;
	glViewport(0, 0, width, height); 
	camera.create_perspective(c_attribs.fov, (float)w_attribs.width / (float)w_attribs.height, c_attribs.near, c_attribs.far);
};

void scroll_callback(GLFWwindow*w, double x_offset, double y_offset)
{
	if (c_attribs.fov + y_offset > 90.f) return;
	if (c_attribs.fov + y_offset < 0.f) return;
	c_attribs.fov += y_offset;
	camera.create_perspective(c_attribs.fov, (float)w_attribs.width / (float)w_attribs.height, c_attribs.near, c_attribs.far);
	printf("FOV : %f\n", c_attribs.fov);
};

int main()
{
	// Init GLFW
	int res = glfwInit();
	if (res == GLFW_FALSE)
	{
		printf("glfw could not be initilaized!\n"); 
		return -1;
	}

	// Create window
	GLFWwindow* window = glfwCreateWindow(w_attribs.width, w_attribs.height, w_attribs.title, 0, 0);
	glfwMakeContextCurrent(window); 

	// Init glad 
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glViewport(0, 0, w_attribs.width, w_attribs.height);

	//Window callbacks : 
	glfwSetWindowSizeCallback(window, &window_resize_callback);
	glfwSetScrollCallback(window, &scroll_callback);

	//Init Figura : 
	fgr::InitFigura(); 

	//Create camera : 	
	camera.create_perspective(c_attribs.fov, (float)w_attribs.width / (float)w_attribs.height, c_attribs.near, c_attribs.far);

	//Create directional light 
	fgr::DirectionalLight light; 
	light.create({ 0,-1,0 }, { 1,1,1 });

	//Load a model : 
	fgr::Model model; 
	model.Load("..\\assets\\argus cam\\argus_cam.obj");
	model.set_position({ 0,0,-5 });

	//Window loop 
	while (!glfwWindowShouldClose(window))
	{
		fgr::ClearColor(0.2f, 0.2f, 0.2f);

		// Move Camera 
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			camera.set_position((camera.get_oreintation() * (c_attribs.speed / w_attribs.fps)) + camera.get_position());
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			camera.set_position(camera.get_position() - (camera.get_oreintation() * (c_attribs.speed / w_attribs.fps)));
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			glm::vec3 movement_vector = glm::cross({ 0,1,0 }, camera.get_oreintation());
			camera.set_position(camera.get_position() + movement_vector * (c_attribs.speed / w_attribs.fps));
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			glm::vec3 movement_vector = glm::cross({ 0,1,0 }, camera.get_oreintation());
			camera.set_position(camera.get_position() - movement_vector * (c_attribs.speed / w_attribs.fps));
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			camera.set_position(camera.get_position() + glm::vec3(0,1,0) * (c_attribs.speed / w_attribs.fps));
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			camera.set_position(camera.get_position() - glm::vec3(0, 1, 0) * (c_attribs.speed / w_attribs.fps));
		}

		// Rotate camera : 
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
		{
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			x -= (float)w_attribs.width / 2.f;
			y -= (float)w_attribs.height / 2.f;
			y = -y;
			x = x / ((float)w_attribs.width / 2.f);
			y = y / ((float)w_attribs.height / 2.f);
			glm::vec4 cam_orientation = glm::vec4(camera.get_oreintation(), 1.f);
			glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
			cam_orientation = glm::rotate(glm::mat4(1.f), c_attribs.sensitivity * -glm::radians((float)x * (c_attribs.fov / 2.f)), up) * cam_orientation;
			cam_orientation = glm::rotate(glm::mat4(1.f), c_attribs.sensitivity * -glm::radians((float)y * (c_attribs.fov / 2.f)), glm::normalize(glm::cross(up, glm::vec3(cam_orientation)))) * cam_orientation;
			camera.face(camera.get_position() + glm::vec3(cam_orientation));
			glfwSetCursorPos(window, (float)w_attribs.width / 2.f, (float)w_attribs.height / 2.f);
		}

		//Draw Models : 
		fgr::default_shader.use(); 
		fgr::default_shader.uniformvec3("viewer_position", camera.get_position().x, camera.get_position().y, camera.get_position().z);
		camera.use(fgr::default_shader);
		model.Draw(fgr::default_shader);

		glfwPollEvents(); 
		glfwSwapBuffers(window);

		_sleep(1000.f / (float)w_attribs.fps);
	}

}