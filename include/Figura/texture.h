#pragma once
#include <glad/glad.h>

namespace fgr {

	class Texture
	{
	private:
		// Texture attribs & data 
		unsigned char* pixels = nullptr; 
		int width, height, nrChannels;		

		// OpenGL : 
		unsigned int _id = 0; 		

	public:		

		void LoadFromFile(const char* path); 		

		bool ready_for_rendering(); 
		bool pixels_stored_in_ram();

		void generate(); 
		void reset(); 
		void bind(GLenum texture_slot = GL_TEXTURE0); 
		static void unbind(GLenum texture_slot = GL_TEXTURE0); 
		
	};
}