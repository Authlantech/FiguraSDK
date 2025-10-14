#include <Figura/model.h>
#include <Figura/Figura.h>

using namespace fgr;

std::queue < std::tuple<std::shared_ptr<fgr::Model>, const char*, std::promise<void>* >> fgr::Model::pending_loads;

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
			load_model(std::get<0>(cl), std::get<1>(cl), std::get<2>(cl));
			pending_loads.pop();
			_sleep(100);
		}
		else
		{
			_sleep(300);
		}
	} while (true);
}

void fgr::Model::load_model(std::shared_ptr<fgr::Model> model, const char* path, std::promise<void>* p)
{
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
		return;
	}

	// Extracting all texture file names: 
	std::vector<std::string> albedo_texture_names;
	std::vector<std::string> normal_texture_names;
	std::vector<std::string> metalness_texture_names;
	std::vector<std::string> roughness_texture_names;
	std::vector<std::string> ao_texture_names;

	for (unsigned int a = 0; a < scene->mNumMaterials; a++)
	{
		aiMaterial* m = scene->mMaterials[a];
		aiString mapname;

		// Extracting albedo maps (checking for PBR base color first, then diffuse): 
		if (m->GetTexture(aiTextureType_BASE_COLOR, 0, &mapname) == AI_SUCCESS || m->GetTexture(aiTextureType_DIFFUSE, 0, &mapname) == AI_SUCCESS)
		{
			std::string name = std::string(mapname.C_Str());
			if (name.find('\\') != std::string::npos) name = name.substr(name.find_last_of('\\') + 1);
			else if (name.find('/') != std::string::npos) name = name.substr(name.find_last_of('/') + 1);

			if (std::find(albedo_texture_names.begin(), albedo_texture_names.end(), name) == albedo_texture_names.end())
			{
				albedo_texture_names.push_back(name);
			}
		}

		// Extracting normal maps: 
		if (m->GetTexture(aiTextureType_NORMALS, 0, &mapname) == AI_SUCCESS)
		{
			std::string name = std::string(mapname.C_Str());
			if (name.find('\\') != std::string::npos) name = name.substr(name.find_last_of('\\') + 1);
			else if (name.find('/') != std::string::npos) name = name.substr(name.find_last_of('/') + 1);

			if (std::find(normal_texture_names.begin(), normal_texture_names.end(), name) == normal_texture_names.end())
			{
				normal_texture_names.push_back(name);
			}
		}

		// Extracting metalness maps:
		if (m->GetTexture(aiTextureType_METALNESS, 0, &mapname) == AI_SUCCESS)
		{
			std::string name = std::string(mapname.C_Str());
			if (name.find('\\') != std::string::npos) name = name.substr(name.find_last_of('\\') + 1);
			else if (name.find('/') != std::string::npos) name = name.substr(name.find_last_of('/') + 1);

			if (std::find(metalness_texture_names.begin(), metalness_texture_names.end(), name) == metalness_texture_names.end())
			{
				metalness_texture_names.push_back(name);
			}
		}

		// Extracting roughness maps:
		if (m->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &mapname) == AI_SUCCESS)
		{
			std::string name = std::string(mapname.C_Str());
			if (name.find('\\') != std::string::npos) name = name.substr(name.find_last_of('\\') + 1);
			else if (name.find('/') != std::string::npos) name = name.substr(name.find_last_of('/') + 1);

			if (std::find(roughness_texture_names.begin(), roughness_texture_names.end(), name) == roughness_texture_names.end())
			{
				roughness_texture_names.push_back(name);
			}
		}

		// Extracting AO maps:
		if (m->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &mapname) == AI_SUCCESS)
		{
			std::string name = std::string(mapname.C_Str());
			if (name.find('\\') != std::string::npos) name = name.substr(name.find_last_of('\\') + 1);
			else if (name.find('/') != std::string::npos) name = name.substr(name.find_last_of('/') + 1);

			if (std::find(ao_texture_names.begin(), ao_texture_names.end(), name) == ao_texture_names.end())
			{
				ao_texture_names.push_back(name);
			}
		}
	}

	// Loading all textures: 
	for (std::string file : albedo_texture_names)
	{
		std::unique_ptr<Texture> texture(new Texture);
		texture->LoadFromFile(std::string(folder + '/' + file).c_str());
		model->albedo_maps.insert({ file, std::move(texture) }); 
	}

	for (std::string file : normal_texture_names)
	{
		std::unique_ptr<Texture> texture(new Texture);
		texture->LoadFromFile(std::string(folder + '/' + file).c_str());
		model->normal_maps.insert({ file, std::move(texture) });
	}

	for (std::string file : metalness_texture_names)
	{
		std::unique_ptr<Texture> texture(new Texture);
		texture->LoadFromFile(std::string(folder + '/' + file).c_str());
		model->metalness_maps.insert({ file, std::move(texture) }); 
	}

	for (std::string file : roughness_texture_names)
	{
		std::unique_ptr<Texture> texture(new Texture);
		texture->LoadFromFile(std::string(folder + '/' + file).c_str());
		model->roughness_maps.insert({ file, std::move(texture) }); 
	}

	for (std::string file : ao_texture_names)
	{
		std::unique_ptr<Texture> texture(new Texture);
		texture->LoadFromFile(std::string(folder + '/' + file).c_str());
		model->ao_maps.insert({ file, std::move(texture) }); 
	}

	processNode(scene->mRootNode, scene, aiMatrix4x4());


	// Loading all meshes: 	

	for (unsigned int a = 0; a < scene->mNumMeshes; a++)
	{
		std::unique_ptr<Mesh> mesh(new Mesh);
		aiMesh* currentMesh = scene->mMeshes[a];

		std::vector<vertex> vertices;
		for (unsigned int b = 0; b < currentMesh->mNumVertices; b++)
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

		aiMaterial* material = scene->mMaterials[currentMesh->mMaterialIndex];
		aiString mapname;

		std::string albedomapname_ = "";
		std::string normalmapname_ = "";
		std::string metalnessmapname_ = "";
		std::string roughnessmapname_ = "";
		std::string aomapname_ = "";

		// Extracting albedo map name if exists 
		if (material->GetTexture(aiTextureType_BASE_COLOR, 0, &mapname) == AI_SUCCESS || material->GetTexture(aiTextureType_DIFFUSE, 0, &mapname) == AI_SUCCESS) {
			std::string dmn(mapname.C_Str());
			if (dmn.find('\\') != std::string::npos) dmn = dmn.substr(dmn.find_last_of('\\') + 1);
			else if (dmn.find('/') != std::string::npos) dmn = dmn.substr(dmn.find_last_of('/') + 1);
			albedomapname_ = dmn;
		}

		// Extracting normal map name if exists
		if (material->GetTexture(aiTextureType_NORMALS, 0, &mapname) == AI_SUCCESS) {
			std::string nmn(mapname.C_Str());
			if (nmn.find('\\') != std::string::npos) nmn = nmn.substr(nmn.find_last_of('\\') + 1);
			else if (nmn.find('/') != std::string::npos) nmn = nmn.substr(nmn.find_last_of('/') + 1);
			normalmapname_ = nmn;
		}

		// Extracting metalness map name if exists
		if (material->GetTexture(aiTextureType_METALNESS, 0, &mapname) == AI_SUCCESS) {
			std::string mn(mapname.C_Str());
			if (mn.find('\\') != std::string::npos) mn = mn.substr(mn.find_last_of('\\') + 1);
			else if (mn.find('/') != std::string::npos) mn = mn.substr(mn.find_last_of('/') + 1);
			metalnessmapname_ = mn;
		}

		// Extracting roughness map name if exists
		if (material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &mapname) == AI_SUCCESS) {
			std::string rn(mapname.C_Str());
			if (rn.find('\\') != std::string::npos) rn = rn.substr(rn.find_last_of('\\') + 1);
			else if (rn.find('/') != std::string::npos) rn = rn.substr(rn.find_last_of('/') + 1);
			roughnessmapname_ = rn;
		}

		// Extracting AO map name if exists
		if (material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &mapname) == AI_SUCCESS) {
			std::string an(mapname.C_Str());
			if (an.find('\\') != std::string::npos) an = an.substr(an.find_last_of('\\') + 1);
			else if (an.find('/') != std::string::npos) an = an.substr(an.find_last_of('/') + 1);
			aomapname_ = an;
		}

		mesh->load(
			vertices,
			indices,
			albedomapname_,
			normalmapname_,
			metalnessmapname_,
			roughnessmapname_,
			aomapname_
		);

		model->meshes.push_back(std::move(mesh));
	}

	p->set_value();
	return;
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

glm::vec3 Model::get_position()
{
	return position;
}

void Model::Render()
{	

	if (loading_thread_checker.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
	{
		for (auto& d : albedo_maps)
			d.second->generate();

		for (auto& n : normal_maps)
			n.second->generate();

		for (auto& n : metalness_maps)
			n.second->generate();

		for (auto& n : roughness_maps)
			n.second->generate();

		for (auto& n : ao_maps)
			n.second->generate();

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

			if (albedo_maps.find(meshes[a]->albedo_map_name) != albedo_maps.end())
			{
				albedo_maps.find(meshes[a]->albedo_map_name)->second->bind(GL_TEXTURE0);
			}

			if (normal_maps.find(meshes[a]->normal_map_name) != normal_maps.end())
			{
				normal_maps.find(meshes[a]->normal_map_name)->second->bind(GL_TEXTURE1);
			}

			if (metalness_maps.find(meshes[a]->metalness_map_name) != metalness_maps.end())
			{
				metalness_maps.find(meshes[a]->metalness_map_name)->second->bind(GL_TEXTURE2);
			}

			if (roughness_maps.find(meshes[a]->roughness_map_name) != roughness_maps.end())
			{
				roughness_maps.find(meshes[a]->roughness_map_name)->second->bind(GL_TEXTURE3);
			}

			if (ao_maps.find(meshes[a]->ao_map_name) != ao_maps.end())
			{
				ao_maps.find(meshes[a]->ao_map_name)->second->bind(GL_TEXTURE4);
			}

			meshes[a]->Render();
		}
	}

}