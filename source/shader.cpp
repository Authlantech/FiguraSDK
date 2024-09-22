#include <Figura/shader.h>

using namespace fgr;

void Shader::create()
{
	program = glCreateProgram(); 
	vshader = glCreateShader(GL_VERTEX_SHADER); 
	fshader = glCreateShader(GL_FRAGMENT_SHADER);
	gshader = glCreateShader(GL_GEOMETRY_SHADER);
}

void Shader::destroy()
{
	glDeleteShader(vshader); 
	glDeleteShader(fshader);
	glDeleteShader(gshader);
	glDeleteProgram(program); 
}

void Shader::load_from_buffer(
	const char* vertex_shader, 
	const char* fragment_shader, 
	const char* geometry_shader
)
{
	glShaderSource(vshader, 1, &vertex_shader, 0);
	glCompileShader(vshader);
	glAttachShader(program, vshader);

	glShaderSource(fshader, 1, &fragment_shader, 0);
	glCompileShader(fshader);
	glAttachShader(program, fshader);

	if (geometry_shader != nullptr)
	{
		glShaderSource(gshader, 1, &geometry_shader, 0);
		glCompileShader(gshader);
		glAttachShader(program, gshader);
	}

	glLinkProgram(program);
}

void Shader::load_from_file(
	const char* vertex_shader_path,
	const char* fragment_shader_path, 
	const char* geometry_shader_path
)
{

}

void Shader::use()
{
	glUseProgram(program);
}



void Shader::uniformmat4f(const char* name, glm::mat4 val)
{
	unsigned int location = glGetUniformLocation(program, name); 
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::uniformfloat(const char* name, float val)
{

}

void Shader::uniformvec3(const char* name, float x, float y, float z) {
	unsigned int location = glGetUniformLocation(program,name);
	glUniform3f(location, x, y, z);
}

void Shader::uniformint(const char* name, int v)
{
	unsigned int location = glGetUniformLocation(program, name);
	glUniform1i(location, v);
}


