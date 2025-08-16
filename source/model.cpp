#include <Figura/model.h>
#include <Figura/Figura.h>

using namespace fgr;

std::queue < std::tuple<fgr::Model*, const char*, std::promise<void>*> > fgr::Model::pending_loads;

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

void Model::model_loading_thread()
{
	do
	{
		if (!pending_loads.empty())
		{
			auto& cl = pending_loads.front();
			load_model(std::get<0>(cl), std::get<1>(cl), std::move(std::get<2>(cl)));
			pending_loads.pop();
			_sleep(100);
		}
		else
		{
			_sleep(300);
		}
	} while (true);
}

void Model::load_model(fgr::Model*model, const char*path, std::promise<void>*p)
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

	std::cout << "\nBEGINING TO LOAD MODEL : " << file_name;
	std::cout << "\n\nDetecting all textures...\n\n";

	for (int a = 0; a < scene->mNumMaterials; a++)
	{
		aiMaterial* m = scene->mMaterials[a];
		aiString diffusemapname;
		aiString normalmapname;

		// Extracting diffuse maps : 
		if (m->GetTexture(aiTextureType_DIFFUSE, 0, &diffusemapname) == AI_SUCCESS)
		{
			std::string name = std::string(diffusemapname.C_Str());
			if (name.find("\\") != std::string::npos) name = name.substr(name.find_last_of('\\') + 1, name.size());
			else if (name.find("/") != std::string::npos) name = name.substr(name.find_last_of('/') + 1, name.size());

			if (std::find(diffuse_texture_names.begin(), diffuse_texture_names.end(), name) == diffuse_texture_names.end())
			{
				std::cout << "\tDIFFUSE MAP : " << name << " detected\n";
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
				std::cout << "\t\tNORMAL MAP : " << name << " detected\n";
				normal_texture_names.push_back(name);
			}
		}
	}

	std::cout << "\tCOMPLETE!\n\nLoading all textures...\n\n";

	// Loading all textures : 
	for (std::string file : diffuse_texture_names)
	{
		Texture* texture = new Texture;
		texture->LoadFromFile(std::string(folder + '\\' + file).c_str());
		model->diffuse_maps.insert({ file, texture });
		std::cout << "\tDIFFUSE MAP : " << file << " is loaded!\n";
	}

	for (std::string file : normal_texture_names)
	{
		Texture* texture = new Texture;
		texture->LoadFromFile(std::string(folder + '\\' + file).c_str());
		model->normal_maps.insert({ file, texture });
		std::cout << "\tNORMAL MAP : " << file << " is loaded!\n";
	}

	std::cout << "\tCOMPLETE!\n\nLoading all meshes...\n";

	processNode(scene->mRootNode, scene, aiMatrix4x4());

	// Loading all meshes : 
	for (int a = 0; a < scene->mNumMeshes; a++)
	{
		Mesh mesh;
		aiMesh* currentMesh = scene->mMeshes[a];

		std::cout << "\n\tMESH : " << currentMesh->mName.C_Str() << "\n\tProperties \n";

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

		std::cout << "\t\tVertex count : " << vertices.size() << "\n\t\tIndex count : " << indices.size() << "\n";

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
			diffusemapname_ = dmn;
			std::cout << "\t\tDiffuse Map : " << diffusemapname_ << std::endl;
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
			normalmapname_ = nmn;
			std::cout << "\t\tNormal Map : " << normalmapname_ << std::endl;

		}

		mesh.load(
			vertices,
			indices,
			diffusemapname_,
			normalmapname_
		);

		model->add_mesh(mesh);
	}

	std::cout << "\n\-----MODEL LOADED!-----\n";

	p->set_value();
	p->~promise(); 

	return;
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
	std::promise<void>* loading_thread_signal = new std::promise<void>;
	loading_thread_checker = loading_thread_signal->get_future();
	pending_loads.push({ this, path, loading_thread_signal });
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

void Model::Render()
{	

	if (loading_thread_checker.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
	{
		for (auto& d : diffuse_maps)
			d.second->generate();

		for (auto& n : normal_maps)
			n.second->generate();

		glm::mat4 modelMatrix = translation * rotation * scaling;
		glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
		fgr::default_window.current_shader.uniformmat4f("modelMatrix", modelMatrix);
		fgr::default_window.current_shader.uniformmat4f("normalMatrix", normalMatrix);
		for (int a = 0; a < meshes.size(); a++)
		{
			fgr::Texture::unbind(GL_TEXTURE0);
			fgr::Texture::unbind(GL_TEXTURE1);

			if (diffuse_maps.find(meshes[a].diffuse_map_name) != diffuse_maps.end())
			{
				diffuse_maps.find(meshes[a].diffuse_map_name)->second->bind(GL_TEXTURE0);
			}

			if (normal_maps.find(meshes[a].normal_map_name) != normal_maps.end())
			{
				normal_maps.find(meshes[a].normal_map_name)->second->bind(GL_TEXTURE1);
			}

			meshes[a].Draw();
		}
	}

}