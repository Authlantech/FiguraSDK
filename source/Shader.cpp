#include <Figura/shader.h>

using namespace fgr;

Shader::Shader()
{
	program = glCreateProgram(); 
	vshader = glCreateShader(GL_VERTEX_SHADER); 
	fshader = glCreateShader(GL_FRAGMENT_SHADER);
	gshader = glCreateShader(GL_GEOMETRY_SHADER);
}

Shader::~Shader()
{
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	glDeleteShader(gshader);
	glDeleteProgram(program);
}

void Shader::LoadFromFile(
	const char* vertex_shader_path, 
	const char* fragment_shader_path, 
	const char* geometry_shader_path
)
{
	// Read vertex shader file
	std::ifstream vs_file(vertex_shader_path);
	std::string vs_source((std::istreambuf_iterator<char>(vs_file)),
		std::istreambuf_iterator<char>());
	vs_file.close();

	// Read fragment shader file
	std::ifstream fs_file(fragment_shader_path);
	std::string fs_source((std::istreambuf_iterator<char>(fs_file)),
		std::istreambuf_iterator<char>());
	fs_file.close();

	// Compile vertex shader
	const char* vs_code = vs_source.c_str();
	glShaderSource(vshader, 1, &vs_code, 0);
	glCompileShader(vshader);
	glAttachShader(program, vshader);

	// Compile fragment shader
	const char* fs_code = fs_source.c_str();
	glShaderSource(fshader, 1, &fs_code, 0);
	glCompileShader(fshader);
	glAttachShader(program, fshader);

	// Compile geometry shader if provided
	if (geometry_shader_path != nullptr)
	{
		std::ifstream gs_file(geometry_shader_path);
		std::string gs_source((std::istreambuf_iterator<char>(gs_file)),
			std::istreambuf_iterator<char>());
		gs_file.close();

		const char* gs_code = gs_source.c_str();
		glShaderSource(gshader, 1, &gs_code, 0);
		glCompileShader(gshader);
		glAttachShader(program, gshader);
	}

	glLinkProgram(program);
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


