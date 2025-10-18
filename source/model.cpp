#include <Figura/model.h>
#include <Figura/Figura.h>

using namespace fgr;

void Model::set_position(glm::vec3 position)
{
	translation = glm::translate(glm::mat4(1.f), position);
	this->position = position;
}

void Model::scale(float v)
{
	scaling = glm::scale(glm::mat4(1.f), glm::vec3(v,v,v));
}

void Model::rotate(glm::vec3 v, float angle)
{
	rotation =  glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::normalize(v)) * rotation;
}

glm::vec3 Model::get_position()
{
	return position;
}

void Model::Load(std::vector<std::shared_ptr<Mesh>> meshes_) {
	meshes = meshes_;
}

void Model::Render()
{
		glm::mat4 modelMatrix = translation * rotation * scaling;
		glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
		fgr::graphic_engine.current_shader->uniformmat4f("modelMatrix", modelMatrix);
		fgr::graphic_engine.current_shader->uniformmat4f("normalMatrix", normalMatrix);
		for (int a = 0; a < meshes.size(); a++)
		{
			fgr::Texture::unbind(GL_TEXTURE0);
			fgr::Texture::unbind(GL_TEXTURE1);
			fgr::Texture::unbind(GL_TEXTURE2);
			fgr::Texture::unbind(GL_TEXTURE3);
			fgr::Texture::unbind(GL_TEXTURE4);

			meshes[a]->Render();
		}
}
