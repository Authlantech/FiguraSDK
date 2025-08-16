#include <Figura/model.h>
#include <Figura/Figura.h>

using namespace fgr;

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

void Model::add_mesh(Mesh mesh)
{
	meshes.push_back(mesh);
}

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

void Model::Load(const char* path)
{
	std::promise<void> loading_thread_signal;
	loading_thread_checker = loading_thread_signal.get_future();

	std::thread(
	[this,path,promise = std::move(loading_thread_signal)]() mutable
		{
			std::string fpath = path;
			std::string folder = fpath.substr(0, fpath.find_last_of('\\'));
			std::string file_name = fpath.substr(fpath.find_last_of('\\') + 1, fpath.size());

			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(fpath, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

			if (scene == nullptr) return;

			// Extracting all texture file names : 
			std::vector<std::string> diffuse_texture_names;
			std::vector<std::string> normal_texture_names;

			std::cout << "[Figura] : Searching and loading all textures\n"; 

			for (int a = 0; a < scene->mNumMaterials; a++)
			{
				aiMaterial* m = scene->mMaterials[a];
				aiString diffusemapname;
				aiString normalmapname;

				// Extracting diffuse maps : 
				if (m->GetTexture(aiTextureType_DIFFUSE, 0, &diffusemapname) == AI_SUCCESS)
				{
					std::string name = std::string(diffusemapname.C_Str());	
					if(name.find("\\") != std::string::npos) name = name.substr(name.find_last_of('\\') + 1, name.size());
					else if(name.find("/") != std::string::npos) name = name.substr(name.find_last_of('/') + 1, name.size());

					if (std::find(diffuse_texture_names.begin(), diffuse_texture_names.end(), name) == diffuse_texture_names.end())
					{			
						std::cout << "Diffuse Map Found -> " << name << std::endl;
						diffuse_texture_names.push_back(name);
					}
				}

				// Extracting normal maps : 
				if (m->GetTexture(aiTextureType_NORMALS, 0, &normalmapname) == AI_SUCCESS)
				{
					std::string name = std::string(normalmapname.C_Str());
					if (name.find("\\") != std::string::npos) name = name.substr(name.find_last_of('\\') + 1, name.size());
					else if (name.find("/") != std::string::npos) name = name.substr(name.find_last_of('/') + 1, name.size());

					if (std::find(normal_texture_names.begin(), normal_texture_names.end(), name) == normal_texture_names.end())
					{
						std::cout << "Normal Map Found ->" << name << std::endl;
						normal_texture_names.push_back(name);
					}
				}
			}

			// Loading all textures : 
			std::map<std::string, Texture> all_diffuse_textures;
			for (std::string file : diffuse_texture_names)
			{
				Texture texture;
				std::cout << "Loading -> " << std::string(folder + '\\' + file).c_str() << std::endl;
				texture.LoadFromFile(std::string(folder + '\\' + file).c_str());
				all_diffuse_textures.insert(std::pair<std::string, Texture>(file, texture));
			}

			std::map<std::string, Texture> all_normal_textures;
			for (std::string file : normal_texture_names)
			{
				Texture texture;
				std::cout << "Loading -> " << std::string(folder + '\\' + file).c_str() << std::endl;
				texture.LoadFromFile(std::string(folder + '\\' + file).c_str());
				all_normal_textures.insert(std::pair<std::string, Texture>(file, texture));
			}

			processNode(scene->mRootNode, scene, aiMatrix4x4());

			// Loading all meshes : 
			for (int a = 0; a < scene->mNumMeshes; a++)
			{
				Mesh mesh;

				aiMesh* currentMesh = scene->mMeshes[a];

				std::vector<vertex> vertices;
				for (int b = 0; b < scene->mMeshes[a]->mNumVertices; b++)
				{
					vertex v;
					v.pos[0] = currentMesh->mVertices[b].x;
					v.pos[1] = currentMesh->mVertices[b].y;
					v.pos[2] = currentMesh->mVertices[b].z;

					if (currentMesh->HasTextureCoords(0))
					{
						v.texture_coordinates[0] = currentMesh->mTextureCoords[0][b].x;
						v.texture_coordinates[1] = currentMesh->mTextureCoords[0][b].y;
					}

					if (scene->mMeshes[a]->HasNormals())
					{
						v.normal[0] = currentMesh->mNormals[b].x;
						v.normal[1] = currentMesh->mNormals[b].y;
						v.normal[2] = currentMesh->mNormals[b].z;
					}

					if (scene->mMeshes[a]->HasTangentsAndBitangents())
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
				for (int b = 0; b < currentMesh->mNumFaces; b++)
				{
					for (int c = 0; c < currentMesh->mFaces[b].mNumIndices; c++)
					{
						indices.push_back(currentMesh->mFaces[b].mIndices[c]);
					}
				}

				aiMaterial* material = scene->mMaterials[currentMesh->mMaterialIndex];
				aiString diffusemapname;
				aiString normalmapname;

				std::string diffusemapname_ = "";
				std::string normalmapname_ = "";

				// Extracting diffuse map name if exists 				
				if (material->GetTexture(aiTextureType_DIFFUSE, 0, &diffusemapname) == AI_SUCCESS) {
					std::string dmn(diffusemapname.C_Str());
					if (dmn.find('\\') != std::string::npos) {
						dmn = dmn.substr(dmn.find_last_of('\\') + 1, dmn.size());
					}
					else if (dmn.find('/') != std::string::npos) {
						dmn = dmn.substr(dmn.find_last_of('/') + 1, dmn.size());
					}
					std::cout << currentMesh->mName.C_Str() << " has diffusemap : " << dmn << std::endl;
					diffusemapname_ = dmn;
				}

				// Extracting normal map name if exists
				if (material->GetTexture(aiTextureType_NORMALS, 0, &normalmapname) == AI_SUCCESS) {
					std::string nmn(normalmapname.C_Str());
					if (nmn.find('\\') != std::string::npos) {
						nmn = nmn.substr(nmn.find_last_of('\\') + 1, nmn.size());
					}
					else if (nmn.find('/') != std::string::npos) {
						nmn = nmn.substr(nmn.find_last_of('/') + 1, nmn.size());
					}
					std::cout << currentMesh->mName.C_Str() << " has normalmap : " << nmn << std::endl;
					normalmapname_ = nmn;
				}

				Texture diffusemap;
				Texture normalmap;											

				if (all_diffuse_textures.find(diffusemapname_) != all_diffuse_textures.end())
				{					
					std::cout << diffusemapname_ << " found among all_diffuse_textures" << std::endl;
					diffusemap = all_diffuse_textures[diffusemapname_];
				}			

				if (all_normal_textures.find(normalmapname_) != all_normal_textures.end())
				{
					std::cout<< normalmapname_ << " found among all_normal_textures" << std::endl;
					normalmap = all_normal_textures[normalmapname_];
				}											
				
				mesh.load(
					vertices,
					indices,
					diffusemap,
					normalmap
				);
				
				this->add_mesh(mesh);

				std::cout << currentMesh->mName.C_Str() << " loaded!\n";
			}				

			std::cout << "\nModel loaded succesfully!\n";

			_sleep(10);

			promise.set_value();
			return;

		}).detach(); 

}

glm::vec3 Model::get_position()
{
	return position;
}

glm::mat4 Model::get_modelMatrix()
{
	return (translation * rotation * scaling);
}

glm::mat4 Model::get_normalMatrix()
{
	return glm::transpose(glm::inverse(get_modelMatrix()));
}

std::vector<fgr::Mesh> Model::get_meshes()
{
	return meshes;
}

void Model::Draw()
{	
	if (loading_thread_checker.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
	{
		glm::mat4 modelMatrix = translation * rotation * scaling;
		glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
		fgr::default_window.current_shader.uniformmat4f("modelMatrix", modelMatrix);
		fgr::default_window.current_shader.uniformmat4f("normalMatrix", normalMatrix);
		for (int a = 0; a < meshes.size(); a++)
		{
			meshes[a].Draw();
		}
	}

}