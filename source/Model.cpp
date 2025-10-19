#include <Figura/model.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <unordered_map>

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

void Model::Render(std::shared_ptr<Shader> shader)
{
		glm::mat4 modelMatrix = translation * rotation * scaling;
		glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
		shader->uniformmat4f("modelMatrix", modelMatrix);
		shader->uniformmat4f("normalMatrix", normalMatrix);
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

void Model::LoadFromData(DATA data) {
	meshes.clear();
	for (auto& d : data.model_meshes) {
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->LoadFromData(d);
		meshes.push_back(mesh);
	}
}


void processNode(aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransformation)
{
	aiMatrix4x4 vertexTransformation = parentTransformation * node->mTransformation;
	aiMatrix3x3 normalTransformation = aiMatrix3x3(vertexTransformation).Inverse().Transpose();

	// Process the mesh :

	for (int a = 0; a < node->mNumMeshes; a++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[a]];

		for (int b = 0; b < mesh->mNumVertices; b++)
		{
			mesh->mVertices[b] = vertexTransformation * mesh->mVertices[b];
			mesh->mNormals[b]  = normalTransformation * mesh->mNormals[b];
			mesh->mTangents[b] = normalTransformation * mesh->mTangents[b];
			mesh->mBitangents[b] = normalTransformation * mesh->mBitangents[b];
		}
	}

	// Process the child nodes :

	for (int a = 0; a < node->mNumChildren; a++)
	{
		processNode(node->mChildren[a], scene, vertexTransformation);
	}

}

Model::DATA Model::LoadModelData(std::string path) {

	DATA loaded_data;

	std::string fpath = path;

	// Convert path format to universal format :

	for (char& c : fpath)
		if (c == '\\') c = '/';

	std::string folder = fpath.substr(0, fpath.find_last_of("/"));
	std::string file_name = fpath.substr(fpath.find_last_of("/") + 1);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fpath, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << importer.GetErrorString() << std::endl;
		return loaded_data;
	}

	// Detecting all Texture files

	aiTextureType desired_texture_types[] = {
		aiTextureType_BASE_COLOR,
		aiTextureType_DIFFUSE,
		aiTextureType_NORMALS,
		aiTextureType_METALNESS,
		aiTextureType_DIFFUSE_ROUGHNESS,
		aiTextureType_AMBIENT_OCCLUSION
	};

	std::unordered_map<std::string,Texture::DATA> loaded_textures;

	for (int a = 0;a < scene->mNumMaterials;a++) {
		aiMaterial* material = scene->mMaterials[a];

		for (const auto& tt : desired_texture_types) {
			int ttc = material->GetTextureCount(tt);
			if (ttc > 0) {
				aiString texture_file_path;
				material->GetTexture(tt,0,&texture_file_path);
				std::string texture_file_name(texture_file_path.C_Str());
				for (auto& c : texture_file_name)
					if (c == '\\') c = '/';

				if (texture_file_name.find('/') != std::string::npos) texture_file_name = texture_file_name.substr(texture_file_name.find_last_of('/') + 1);
				loaded_textures[texture_file_name] = {};
			}
		}
	}

	// Load all textures :

	for (auto& t : loaded_textures) {
		std::string full_texture_path = folder + '/' + t.first;
		t.second = Texture::LoadTextureData(full_texture_path.c_str());
	}

	processNode(scene->mRootNode, scene, aiMatrix4x4());

	// Loading all meshes:

	for (unsigned int a = 0; a < scene->mNumMeshes; a++)
	{
		Mesh::DATA mesh_data;

		aiMesh* currentMesh = scene->mMeshes[a];

		std::vector<Vertex> vertices;
		for (unsigned int b = 0; b < currentMesh->mNumVertices; b++)
		{
			Vertex v;
			v.position[0] = currentMesh->mVertices[b].x;
			v.position[1] = currentMesh->mVertices[b].y;
			v.position[2] = currentMesh->mVertices[b].z;

			if (currentMesh->HasTextureCoords(0))
			{
				v.texture_coordinates[0] = currentMesh->mTextureCoords[0][b].x;
				v.texture_coordinates[1] = currentMesh->mTextureCoords[0][b].y;
			}
			if (currentMesh->HasNormals())
			{
				v.normal[0] = currentMesh->mNormals[b].x;
				v.normal[1] = currentMesh->mNormals[b].y;
				v.normal[2] = currentMesh->mNormals[b].z;
			}
			if (currentMesh->HasTangentsAndBitangents())
			{
				v.tangent[0] = currentMesh->mTangents[b].x;
				v.tangent[1] = currentMesh->mTangents[b].y;
				v.tangent[2] = currentMesh->mTangents[b].z;

				v.bittangent[0] = currentMesh->mBitangents[b].x;
				v.bittangent[1] = currentMesh->mBitangents[b].y;
				v.bittangent[2] = currentMesh->mBitangents[b].z;
			}
			vertices.push_back(v);
		}

		std::vector<unsigned int> indices;
		for (unsigned int b = 0; b < currentMesh->mNumFaces; b++)
		{
			for (unsigned int c = 0; c < currentMesh->mFaces[b].mNumIndices; c++)
			{
				indices.push_back(currentMesh->mFaces[b].mIndices[c]);
			}
		}

		mesh_data.vertices = vertices;
		mesh_data.indices =  indices;

		// Get mesh's textures

		aiMaterial* material = scene->mMaterials[currentMesh->mMaterialIndex];

		for (const auto& tt : desired_texture_types) {
			int ttc = material->GetTextureCount(tt);
			if (ttc > 0) {
				aiString texture_file_path;
				material->GetTexture(tt,0,&texture_file_path);
				std::string texture_file_name(texture_file_path.C_Str());
				for (auto& c : texture_file_name)
					if (c == '\\') c = '/';

				if (loaded_textures.find(texture_file_name) != loaded_textures.end()) {
					mesh_data.textures.push_back(loaded_textures[texture_file_name]);
				}
			}
		}

		loaded_data.model_meshes.push_back(mesh_data);
	}

	return loaded_data;
}