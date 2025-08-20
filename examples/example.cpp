#include <Figura/Figura.h>
#include <GLFW/glfw3.h>

#include <iostream> 
#include <vector>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// Window attribs
struct WindowAttribs
{
	int width = 1200;
	int height = 800;
	const char* title = "example";
	int fps = 144;
}w_attribs;

// Camera attribs  
struct CameraAttribs
{
	float fov = 60.f; 
	float speed = 50.f; 
	float sensitivity = 4.f;
	float near = 0.01f; 
	float far = 1000.f;
}c_attribs;

int main()
{
	//Init Figura : 
	fgr::graphic_engine.init_engine(w_attribs.width,w_attribs.height,w_attribs.title);

	GLFWwindow* window = fgr::graphic_engine._get_window();

	//Init Imgui : 
	
	IMGUI_CHECKVERSION(); 
	ImGui::CreateContext(); 
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	//Create camera : 	
	fgr::graphic_engine.create_perspective_camera("default camera", c_attribs.fov, (float)w_attribs.width / (float)w_attribs.height, c_attribs.near, c_attribs.far);

	//Create a Light : 
	fgr::graphic_engine.create_directional_light("sun", { 1,-1,0 }, { 1,1,1 }); 

	//Window loop 
	while (fgr::graphic_engine.window_is_open())
	{
		fgr::graphic_engine.clear_window(0.2,0.2,0.8,0);

		// Move Camera 
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			fgr::graphic_engine.get_camera("default camera")->set_position((fgr::graphic_engine.get_camera("default camera")->get_oreintation() * (c_attribs.speed / w_attribs.fps)) + fgr::graphic_engine.get_camera("default camera")->get_position());
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			fgr::graphic_engine.get_camera("default camera")->set_position(fgr::graphic_engine.get_camera("default camera")->get_position() - (fgr::graphic_engine.get_camera("default camera")->get_oreintation() * (c_attribs.speed / w_attribs.fps)));
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			glm::vec3 movement_vector = glm::cross({ 0,1,0 }, fgr::graphic_engine.get_camera("default camera")->get_oreintation());
			fgr::graphic_engine.get_camera("default camera")->set_position(fgr::graphic_engine.get_camera("default camera")->get_position() + movement_vector * (c_attribs.speed / w_attribs.fps));
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			glm::vec3 movement_vector = glm::cross({ 0,1,0 }, fgr::graphic_engine.get_camera("default camera")->get_oreintation());
			fgr::graphic_engine.get_camera("default camera")->set_position(fgr::graphic_engine.get_camera("default camera")->get_position() - movement_vector * (c_attribs.speed / w_attribs.fps));
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			fgr::graphic_engine.get_camera("default camera")->set_position(fgr::graphic_engine.get_camera("default camera")->get_position() + glm::vec3(0,1,0) * (c_attribs.speed / w_attribs.fps));
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			fgr::graphic_engine.get_camera("default camera")->set_position(fgr::graphic_engine.get_camera("default camera")->get_position() - glm::vec3(0, 1, 0) * (c_attribs.speed / w_attribs.fps));
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
			glm::vec4 cam_orientation = glm::vec4(fgr::graphic_engine.get_camera("default camera")->get_oreintation(), 1.f);
			glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
			cam_orientation = glm::rotate(glm::mat4(1.f), c_attribs.sensitivity * -glm::radians((float)x * (c_attribs.fov / 2.f)), up) * cam_orientation;
			cam_orientation = glm::rotate(glm::mat4(1.f), c_attribs.sensitivity * -glm::radians((float)y * (c_attribs.fov / 2.f)), glm::normalize(glm::cross(up, glm::vec3(cam_orientation)))) * cam_orientation;
			fgr::graphic_engine.get_camera("default camera")->face(fgr::graphic_engine.get_camera("default camera")->get_position() + glm::vec3(cam_orientation));
			glfwSetCursorPos(window, (float)w_attribs.width / 2.f, (float)w_attribs.height / 2.f);
		}		

		
		fgr::graphic_engine.get_camera("default camera")->use();

		for (const auto& n : fgr::graphic_engine.get_model_names())
		{
			fgr::graphic_engine.get_model(n)->Render();
		}

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (ImGui::Begin("Models"))
		{
			static char name[100] = {0};
			static char path[200] = {0};
			ImGui::InputText("Name", name, 100);
			ImGui::InputText("Path", path, 200);
			if (*path && *name)
			{
				if (ImGui::Button("Load"))
				{
					fgr::graphic_engine.create_model(name, path);
				}
			}

			if (ImGui::BeginTable("Models", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
			{
				// Set up headers
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Position");
				ImGui::TableHeadersRow();

				for (const auto n : fgr::graphic_engine.get_model_names())
				{
					auto& model = fgr::graphic_engine.get_model(n);

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s", n.c_str());

					ImGui::TableSetColumnIndex(1);
					static float pos[3] = { 0 };
					pos[0] = model->get_position().x;
					pos[1] = model->get_position().y;
					pos[2] = model->get_position().z;
					ImGui::DragFloat3(n.c_str(), pos);
					model->set_position({ pos[0], pos[1], pos[2] });
				}

				ImGui::EndTable();
			}


			ImGui::End();
		}


		if (ImGui::Begin("Lights"))
		{
			static char name[100] = {0};
			ImGui::InputText("Name", name,100);
			if (*name)
			{
				if (ImGui::Button("create"))
				{
					fgr::graphic_engine.create_point_light(name, { 0,0,0 }, { 1,1,1 }); 
					memset(name, 0, 100);
				}
			}

			if (ImGui::BeginTable("Point Lights", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
			{
				// Set up headers
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Position");
				ImGui::TableSetupColumn("Color");
				ImGui::TableHeadersRow();

				for (const auto n : fgr::graphic_engine.get_point_light_names())
				{
					auto& light = fgr::graphic_engine.get_point_light(n);

					ImGui::TableNextRow(); 
					ImGui::TableSetColumnIndex(0); 
					ImGui::Text("%s", n.c_str());

					ImGui::TableSetColumnIndex(1);
					static float pos[3] = { 0 }; 
					pos[0] = light->get_position().x; 
					pos[1] = light->get_position().y; 
					pos[2] = light->get_position().z; 
					ImGui::DragFloat3(n.c_str(), pos);
					light->set_position({ pos[0],pos[1],pos[2] });

					ImGui::TableSetColumnIndex(2);
					static float color[3] = { 0 }; 
					color[0] = light->get_color().x;
					color[1] = light->get_color().y;
					color[2] = light->get_color().z;
					ImGui::ColorEdit3(n.c_str(), color); 
					light->set_color({ color[0], color[1], color[2] });
				}

				ImGui::EndTable();
			}

			ImGui::End();
		}

		if (ImGui::Begin("Shaders"))
		{
			if (ImGui::BeginListBox("Shaders"))
			{
				const auto s_names = fgr::graphic_engine.get_shader_names(); 
				static int selected = 0; 

				for (int a = 0;a < s_names.size();a++)
				{
					if (ImGui::Selectable(s_names[a].c_str(), selected == a))
					{
						selected = a;
					}
				}

				fgr::graphic_engine.use_shader(s_names[selected]);

				ImGui::EndListBox();
			}

			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		fgr::graphic_engine.update_window();

		_sleep(1000.f / (float)w_attribs.fps);
	}

}