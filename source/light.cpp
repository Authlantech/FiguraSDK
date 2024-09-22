#include <Figura/light.h>

namespace fgr
{
	unsigned int DirectionalLight::d_light_buffer;
	std::vector<DirectionalLight*> DirectionalLight::d_light_vector;

	void DirectionalLight::begin_directional_lights()
	{
		glGenBuffers(1, &d_light_buffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, d_light_buffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, d_light_buffer);
	}

	void DirectionalLight::update()
	{
		
		// Update light vector : 

		std::vector<DirectionalLight*> temp;
		std::vector<DirectionalLight::ATTRIBS> data;
		for (int a = 0; a < d_light_vector.size(); a++)
		{
			if (d_light_vector[a]->exist == true)
			{
				temp.push_back(d_light_vector[a]);
				data.push_back(d_light_vector[a]->attribs);
			}
		}

		d_light_vector.clear();
		d_light_vector = temp;

		//Update light buffer : 

		int light_count = d_light_vector.size();

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, d_light_buffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) + sizeof(ATTRIBS) * light_count, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), &light_count);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(int), sizeof(ATTRIBS) * light_count, data.data());		
	}

	void DirectionalLight::create(glm::vec3 direction, glm::vec3 color)
	{
		this->attribs.d_x = direction.x;
		this->attribs.d_y = direction.y;
		this->attribs.d_z = direction.z;

		this->attribs.r = color.r;
		this->attribs.g = color.g;
		this->attribs.b = color.b;

		this->exist = true;
		d_light_vector.push_back(this);
		update();
	}

	void DirectionalLight::destroy()
	{
		exist = false;
		update();
	}

	void DirectionalLight::set_direction(glm::vec3 direction)
	{
		this->attribs.d_x = direction.x;
		this->attribs.d_y = direction.y;
		this->attribs.d_z = direction.z;
		update();
	}

	void DirectionalLight::set_color(glm::vec3 color)
	{
		this->attribs.r = color.r;
		this->attribs.g = color.g;
		this->attribs.b = color.b;
		update();
	}

	glm::vec3 DirectionalLight::get_direction()
	{
		return glm::vec3();
	}

	glm::vec3 DirectionalLight::get_color()
	{
		return glm::vec3();
	}


	std::vector<PointLight*> PointLight::p_light_vector; 
	unsigned int PointLight::p_light_buffer;

	void PointLight::begin_point_lights()
	{
		glGenBuffers(1, &p_light_buffer); 
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, p_light_buffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, p_light_buffer);
	}

	void PointLight::update()
	{
		// Update light vector : 

		std::vector<PointLight*> temp; 
		std::vector<ATTRIBS> data;
		for (int a = 0; a < p_light_vector.size(); a++)
		{
			if (p_light_vector[a]->exist == true)
			{
				temp.push_back(p_light_vector[a]);
				data.push_back(p_light_vector[a]->attribs);
			}
		}

		p_light_vector.clear();
		p_light_vector = temp;

		//Update light buffer : 

		int light_count = p_light_vector.size();

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, p_light_buffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) + sizeof(ATTRIBS) * light_count, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), &light_count);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(int), sizeof(ATTRIBS) * light_count, data.data());
	}

	void PointLight::create(glm::vec3 position, glm::vec3 color)
	{
		attribs.x = position.x; 
		attribs.y = position.y;
		attribs.z = position.z;

		attribs.r = color.r; 
		attribs.g = color.g; 
		attribs.b = color.b; 

		this->exist = true; 
		p_light_vector.push_back(this);
		update();
	}

	void PointLight::destroy()
	{
		this->exist = false; 
		update();
	}

	void PointLight::set_position(glm::vec3 position)
	{
		attribs.x = position.x; 
		attribs.y = position.y; 
		attribs.z = position.z; 
		update();
	}

	void PointLight::set_color(glm::vec3 color)
	{
		attribs.r = color.r; 
		attribs.g = color.g; 
		attribs.b = color.b; 
		update();
	}

	glm::vec3 PointLight::get_position()
	{
		return glm::vec3(attribs.x, attribs.y, attribs.z);
	}

	glm::vec3 PointLight::get_color()
	{
		return glm::vec3(attribs.r, attribs.g, attribs.b);
	}


	std::vector<SpotLight*> SpotLight::s_light_vector;
	unsigned int SpotLight::s_light_buffer;

	void SpotLight::begin_spot_light()
	{
		glGenBuffers(1, &s_light_buffer); 
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, s_light_buffer); 
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, s_light_buffer);
	}

	void SpotLight::update()
	{	
		// Update light vector : 
		
		std::vector<SpotLight*> temp; 
		std::vector<ATTRIBS> data; 

		for (int a = 0; a < s_light_vector.size(); a++)
		{
			if (s_light_vector[a]->exist == true)
			{
				temp.push_back(s_light_vector[a]);
				data.push_back(s_light_vector[a]->attribs);
			}
		}

		s_light_vector.clear(); 
		s_light_vector = temp;

		// Update light buffer : 

		int s_count = s_light_vector.size(); 
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, s_light_buffer); 
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) + sizeof(ATTRIBS) * s_count, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), &s_count);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(int), sizeof(ATTRIBS) * s_count, data.data());
	}

	void SpotLight::create(glm::vec3 position, glm::vec3 color, glm::vec3 orientation, float angle)
	{
		attribs.x = position.x;
		attribs.y = position.y;
		attribs.z = position.z;

		attribs.r = color.r; 
		attribs.g = color.g; 
		attribs.b = color.b; 

		attribs.o_x = orientation.x; 
		attribs.o_y = orientation.y; 
		attribs.o_z = orientation.z; 

		attribs.angle = angle;
		
		s_light_vector.push_back(this);
		this->exist = true;
		update();
	}

	void SpotLight::destroy()
	{
		this->exist = false; 
		update();
	}

	void SpotLight::set_position(glm::vec3 position)
	{
		attribs.x = position.x;
		attribs.y = position.y;
		attribs.z = position.z;
		update();
	}

	void SpotLight::set_color(glm::vec3 color)
	{
		attribs.r = color.r;
		attribs.g = color.g;
		attribs.b = color.b;
		update();
	}

	void SpotLight::set_orientation(glm::vec3 orientation)
	{
		attribs.o_x = orientation.x;
		attribs.o_y = orientation.y;
		attribs.o_z = orientation.z;
		update();
	}

	void SpotLight::set_angle(float angle)
	{
		attribs.angle = angle;
		update();
	}

	glm::vec3 SpotLight::get_position()
	{
		return glm::vec3(attribs.x, attribs.y, attribs.z);
	}

	glm::vec3 SpotLight::get_color()
	{
		return glm::vec3(attribs.r, attribs.g, attribs.b);
	}

	glm::vec3 SpotLight::get_orientation()
	{
		return glm::vec3(attribs.o_x, attribs.o_y, attribs.o_z);
	}

	float SpotLight::get_angle()
	{
		return attribs.angle;
	}



}