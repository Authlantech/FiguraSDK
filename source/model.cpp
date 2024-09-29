#include <Figura/model.h>

using namespace fgr;

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

bool Model::Load(const char* path)
{
	std::string fpath = path;
	std::string folder = fpath.substr(0,fpath.find_last_of('\\'));
	std::string file_name = fpath.substr(fpath.find_last_of('\\') + 1,fpath.size());	

	Assimp::Importer importer; 
	const aiScene* scene = importer.ReadFile(fpath, aiProcess_Triangulate | aiProcess_GenNormals); 			

	if (scene == nullptr) return false;


	printf("\n<MODEL> : %s\n", file_name.c_str());

	//Extracting all texture file names : 

	std::vector<std::string> texture_names; 

	for (int a = 0; a < scene->mNumMaterials; a++)
	{
		aiMaterial* m = scene->mMaterials[a];		
		aiString diffusemapname;		
		aiString specularmapname;
		aiString metalnessmapname;

		// Extracting diffuse maps : 

		if (m->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &diffusemapname) == AI_SUCCESS)
		{
			std::string name = std::string(diffusemapname.C_Str());
			name = name.substr(name.find_last_of('\\') + 1, name.size());			

			printf("\t<DIFFUSE MAP> : %s\n", name.c_str());

			// Adding file name to list :

			if (std::find(texture_names.begin(), texture_names.end(), name) == texture_names.end())
			{
				texture_names.push_back(name);
			}
		}

		//Extracting sepcular maps : 

		if (m->GetTexture(aiTextureType::aiTextureType_SPECULAR, 0, &specularmapname) == AI_SUCCESS)
		{
			std::string name = std::string(specularmapname.C_Str());
			name = name.substr(name.find_last_of('\\') + 1, name.size());

			printf("\t<SPECULAR MAP> : %s\n", name.c_str());


			// Adding file name to list :

			if (std::find(texture_names.begin(), texture_names.end(), name) == texture_names.end())
			{
				texture_names.push_back(name);
			}
		}

		//Extracting metalness maps : 

		if (m->GetTexture(aiTextureType::aiTextureType_METALNESS, 0, &metalnessmapname) == AI_SUCCESS)
		{
			std::string name = std::string(metalnessmapname.C_Str());
			name = name.substr(name.find_last_of('\\') + 1, name.size());

			printf("\t<METALNESS MAP> : %s\n", name.c_str());

			// Adding file name to list :

			if (std::find(texture_names.begin(), texture_names.end(), name) == texture_names.end())
			{
				texture_names.push_back(name);
			}
		}

	}

	// Loading all textures : 

	std::map<std::string, Texture> all_textures;
	for (std::string file : texture_names)
	{
		Texture texture; 
		texture.generate();		
		texture.load(std::string(folder + '\\' + file).c_str());
		all_textures.insert(std::pair<std::string,Texture>(file,texture));
	}

	// Loading all meshes : 

	for (int a = 0; a < scene->mNumMeshes; a++)
	{		
		Mesh mesh; 
		mesh.generate();		

		aiMesh* currentMesh = scene->mMeshes[a];

		// Looping through every vertex : 

		std::vector<vertex> vertices; 
		for (int b = 0; b < scene->mMeshes[a]->mNumVertices; b++)
		{
			// Loading vertex position : 

			vertex v;
			v.pos[0] = currentMesh->mVertices[b].x;
			v.pos[1] = currentMesh->mVertices[b].y;
			v.pos[2] = currentMesh->mVertices[b].z;						

			// Loading texture coordinates if exists :

			if (currentMesh->HasTextureCoords(0))
			{				
				v.texture_coordinates[0] = currentMesh->mTextureCoords[0][b].x;
				v.texture_coordinates[1] = currentMesh->mTextureCoords[0][b].y;
			}

			// Loading vertex normal if exists :

			if (scene->mMeshes[a]->HasNormals())
			{
				v.normals[0] = currentMesh->mNormals[b].x;
				v.normals[1] = currentMesh->mNormals[b].y;
				v.normals[2] = currentMesh->mNormals[b].z;
			}

			vertices.push_back(v);
		}

		std::vector<unsigned int> indices;

		// Loading indices : 

		for (int b = 0; b < currentMesh->mNumFaces; b++)
		{		
			for (int c = 0; c < currentMesh->mFaces[b].mNumIndices; c++)
			{
				indices.push_back(currentMesh->mFaces[b].mIndices[c]);
			}
		}

		// Loading material

		aiMaterial* material = scene->mMaterials[currentMesh->mMaterialIndex];
		aiString diffusemapname;		
		aiString specularmapname;
		aiString metalnessmapname;

		std::string diffusemapname_, specularmapname_, metalnessmapname_;

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

		}

		// Extracting specular map name if exists 				

		if (material->GetTexture(aiTextureType_SPECULAR, 0, &specularmapname) == AI_SUCCESS) {

			std::string smn(specularmapname.C_Str());

			if (smn.find('\\') != std::string::npos) {
				smn = smn.substr(smn.find_last_of('\\') + 1, smn.size());
			}

			else if (smn.find('/') != std::string::npos) {
				smn = smn.substr(smn.find_last_of('/') + 1, smn.size());
			}

			specularmapname_ = smn;

		}

		// Extracting metalness map name if exists 				

		if (material->GetTexture(aiTextureType_METALNESS, 0, &metalnessmapname) == AI_SUCCESS) {

			std::string mmn(metalnessmapname.C_Str());

			if (mmn.find('\\') != std::string::npos) {
				mmn = mmn.substr(mmn.find_last_of('\\') + 1, mmn.size());
			}

			else if (mmn.find('/') != std::string::npos) {
				mmn = mmn.substr(mmn.find_last_of('/') + 1, mmn.size());
			}

			metalnessmapname_ = mmn;

		}

		Texture diffusemap;
		Texture specularmap;
		Texture metalicmap;
		
		if (all_textures.find(diffusemapname_) != all_textures.end())
		{
			diffusemap = all_textures.find(diffusemapname_)._Ptr->_Myval.second;
		}

		if (all_textures.find(specularmapname_) != all_textures.end())
		{
			specularmap = all_textures.find(specularmapname_)._Ptr->_Myval.second;
		}

		if (all_textures.find(metalnessmapname_) != all_textures.end())
		{
			metalicmap = all_textures.find(metalnessmapname_)._Ptr->_Myval.second;
		}

		mesh.load(
			vertices.data(), vertices.size() * sizeof(vertex),
			indices.data(), indices.size() * sizeof(unsigned int),
			diffusemap,
			specularmap,
			metalicmap
		);

		this->add_mesh(mesh);
	}
	
	return true;

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

void Model::Draw(Shader shader)
{
	glm::mat4 modelMatrix = translation * rotation * scaling;
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
	shader.uniformmat4f("modelMatrix", modelMatrix); 
	shader.uniformmat4f("normalMatrix", normalMatrix);
	for (int a = 0;a < meshes.size();a++)
	{
		meshes[a].Draw(shader);
	}
}