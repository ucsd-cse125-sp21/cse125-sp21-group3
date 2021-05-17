#include "TextureCube.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/gtx/string_cast.hpp>

/*
 * File Name: Cube.cpp
 *
 * This file contains the Cube class implementation. This is where any methods
 * related to drawing, updating, or creating cubes should be written.
 *
 * @author Part of 169 starter code
 */
////////////////////////////////////////////////////////////////////////////////

/*
 * Cube constructor which creates a cube given two points and sets up its OpenGL variables.
 *
 * @param cubeMin The minimum corner of the cube (can be thought of as bottom left corner)
 * @param cubeMax The maximum corner of the cube (can be thought of as top right corner)
 * @return new Cube object
 * @author Part of 169 starter code
 */
TextureCube::TextureCube() 
{
	// Model matrix.
	model = glm::mat4(1.0f);
	
	// The color of the cube. Try setting it to something else!
	color = glm::vec3(1.0f, 0.56f, 0.0f); 

	// Specify vertex positions
	positions = {
		glm::vec3(2.5f,  2.5f, -3.0f),
		glm::vec3(2.5f, 1.5f, -3.0f),
		glm::vec3(1.5f, 1.5f, -3.0f),
		glm::vec3(1.5f,  2.5f, -3.0f)
	};

	// Specify normals
	textureCoords = {
		 glm::vec2(0.0f, 0.0f), // top  right
		 glm::vec2(0.0f, 1.0f), // bottom right
		 glm::vec2(1.0f, 1.0f), // bottom left
		 glm::vec2(1.0f, 0.0f) //top left
	};
	
	// Specify indices
	indices = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};


	// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_positions);
	glGenBuffers(1, &VBO_textureCoords);

	// Bind to the VAO.
	glBindVertexArray(VAO);

	// Bind to the first VBO - We will use it to store the vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind to the second VBO - We will use it to store the normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_textureCoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * textureCoords.size(), textureCoords.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

	// Generate EBO, bind the EBO to the bound VAO and send the data
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Unbind the VBOs.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char* data = stbi_load("Assets/icons/increasePlayerMaxHealth.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	
}

////////////////////////////////////////////////////////////////////////////////
/*
 * Cube destructor. Frees up memory associated with OpenGL variables.
 *
 * @author Part of 169 starter code
 */
TextureCube::~TextureCube()
{
	
	// Delete the VBOs and the VAO.
	glDeleteBuffers(1, &VBO_positions);
	glDeleteBuffers(1, &VBO_textureCoords);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);

	std::cout << "Deleting" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
/*
 * Renders the cube.
 *
 * @param viewProjMtx The view projection matrix needed to render the cube to the window.
 * Check the rendering reference section of the project documentation for more details.
 * @param shader The shader used to render the cube
 * @author Part of 169 starter code
 */
void TextureCube::draw(const glm::mat4& viewProjMtx, GLuint shader)
{

	glm::vec3 position = glm::vec3(3.0f, 3.5f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 3.5, 0.0f - position.z);
	float FOV = 45.0f;
	float Aspect = 1.33f;
	float NearClip = 0.01f;
	float FarClip = 150.0f;
	glm::mat4 view = glm::lookAt(
		position,
		position + cameraFront,
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	// Compute perspective projection matrix, used to project points in view space onto flat plane
	glm::mat4 project = glm::perspective(glm::radians(FOV), Aspect, NearClip, FarClip);

	// Compute final view-projection matrix, final positions which are used
	glm::mat4 ViewProjectMtx = {
		{1.81066f, 0.0f, 0.0f, 0.0f},
		{0.0f, 2.41423f, 0.0f, 0.0f},
		{0.0f, 0.0f, -1.00133f, -1.0f},
		{-4.52f, -8.470871f, 2.480332f, 2.5f}
	};

	// activate the shader program 
	glUseProgram(shader);

	glBindTexture(GL_TEXTURE_2D, texture);
	//cout << "calculated: " << glm::to_string(ViewProjectMtx) << endl;
	//cout << "real: " << glm::to_string(viewProjMtx) << endl;
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&ViewProjectMtx);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
	
	// Bind the VAO
	glBindVertexArray(VAO);

	// draw the points using triangles, indexed with the EBO
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
