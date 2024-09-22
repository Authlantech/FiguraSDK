#include <glad/glad.h>
#include <glm/glm.hpp>

#include <iostream> 
#include <vector> 

namespace fgr
{

	class DirectionalLight 
	{

		static std::vector<DirectionalLight*> d_light_vector;
		static unsigned int d_light_buffer;
	private : 
		struct ATTRIBS
		{
			float d_x, d_y, d_z; // Direction
			float r, g, b;		 // Color
		}attribs = {0.0, 0.0, 0.0,  0.0, 0.0, 0.0};
		bool exist = true;
		static void update(); 
	public :
		static void begin_directional_lights();

		void create(glm::vec3 direction, glm::vec3 color); 
		void destroy();

		void set_direction(glm::vec3 direction); 
		void set_color(glm::vec3 color); 

		glm::vec3 get_direction(); 
		glm::vec3 get_color();

	};

	class PointLight
	{
		static std::vector<PointLight*> p_light_vector; 
		static unsigned int p_light_buffer; 
	private : 
		struct ATTRIBS
		{
			float x, y, z; 
			float r, g, b;
		}attribs;
		bool exist = true;
		static void update();
	public : 
		static void begin_point_lights();

		void create(glm::vec3 position,glm::vec3 color); 
		void destroy(); 

		void set_position(glm::vec3 position);
		void set_color(glm::vec3 color); 

		glm::vec3 get_position(); 
		glm::vec3 get_color();
	};


	class SpotLight 
	{
		static std::vector<SpotLight*> s_light_vector; 
		static unsigned int s_light_buffer; 
	private : 
		struct ATTRIBS
		{
			float x, y, z; 
			float r, g, b; 
			float o_x, o_y, o_z; 
			float angle;
		}attribs;
		bool exist = true; 
		static void update();
	public : 
		static void begin_spot_light();
		
		void create(glm::vec3 position, glm::vec3 color, glm::vec3 orientation, float angle);
		void destroy(); 

		void set_position(glm::vec3 position); 
		void set_color(glm::vec3 color); 
		void set_orientation(glm::vec3 orientation);
		void set_angle(float angle);

		glm::vec3 get_position(); 
		glm::vec3 get_color(); 
		glm::vec3 get_orientation();
		float get_angle();

	};
}