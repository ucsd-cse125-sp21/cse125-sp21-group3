#include "Mesh.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

/*
 * File Name: Mesh.cpp
 *
 * This file contains the Mesh implementation. This class will handle
 * rendering the models loaded in via ASSIMP and the Model class.
 *
 * @author Lucas Hwang
 */

 ////////////////////////////////////////////////////////////////////////////////

Mesh::Mesh(aiMesh* aiMesh, const aiScene* scene, glm::mat4 _model)
{
	// walk through each of the mesh's vertices
	for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
	{
		positions.push_back(glm::vec3(aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z));
		// normals
		if (aiMesh->HasNormals())
		{
			normals.push_back(glm::vec3(aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z));
		}
	}

	initialPositions = positions;
	initialNormals = normals;

	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
	{
		aiFace face = aiMesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[aiMesh->mMaterialIndex];

	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps

	aiColor4D color(1.0f, 0.0f, 0.0f, 1.0f);
	material->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR, color);

	//add bones to mesh
	for (int i = 0; i < aiMesh->mNumBones; i++) {
		aiBone* aiBone = aiMesh->mBones[i];
		bones.push_back(new Bone(aiBone, positions, normals));
	}


	//initialize bone positions and bone normals
	for (int i = 0; i < positions.size(); i++) {
		bonePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		boneNormals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	}

	model = _model;
	baseColor = glm::vec4(color.r, color.g, color.b, color.a);
	name = aiMesh->mName.C_Str();
	id = 0;
    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    setupMesh();
}

void Mesh::draw(const glm::mat4& viewProjMtx, GLuint shader)
{
	glUseProgram(shader);

	// get the locations and send the uniforms to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
	glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &baseColor[0]);

	// Bind the VAO
	glBindVertexArray(VAO);

	// draw the points using triangles, indexed with the EBO
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);

    // always good practice to set everything back to defaults once configured.
    //glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
	// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_positions);
	glGenBuffers(1, &VBO_normals);

	// Bind to the VAO.
	glBindVertexArray(VAO);

	// Bind to the first VBO - We will use it to store the vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind to the second VBO - We will use it to store the normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Generate EBO, bind the EBO to the bound VAO and send the data
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Unbind the VBOs.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void Mesh::clearBoneData() {

	bonePositions.clear();
	boneNormals.clear();
	for (int i = 0; i < positions.size(); i++) {
		bonePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		boneNormals.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	}

}
////////////////////////////////////////////////////////////////////////////////
