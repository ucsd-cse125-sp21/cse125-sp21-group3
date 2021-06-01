#include "Cube.h"
#include "Window.h"
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
Cube::Cube(glm::vec3 cubeMin, glm::vec3 cubeMax, int type, bool client) 
{

	// Model matrix.
	model = glm::mat4(1.0f);
	
	// The color of the cube. Try setting it to something else!
	color = glm::vec3(1.0f, 0.56f, 0.0f); 

	isClient = client;


	//bounding box setup
	boundingBox = new BoundingBox(cubeMin, cubeMax, this, isClient);

	mazePosition = cubeMin;

	if (cubeMax[0] - cubeMin[0] < 1.0f)
	{
		direction = true; // bottom
	}
	else
	{
		direction = false; // right
	}

	if (type == wall)
	{
		canDelete = true;
	}
	else
	{
		canDelete = false;
	}

	cubeType = type;

	// Specify vertex positions
	positions = {
		// Front
		glm::vec3(cubeMin.x,cubeMin.y,cubeMax.z),
		glm::vec3(cubeMax.x,cubeMin.y,cubeMax.z),
		glm::vec3(cubeMax.x,cubeMax.y,cubeMax.z),
		glm::vec3(cubeMin.x,cubeMax.y,cubeMax.z),

		// Back
		glm::vec3(cubeMax.x,cubeMin.y,cubeMin.z),
		glm::vec3(cubeMin.x,cubeMin.y,cubeMin.z),
		glm::vec3(cubeMin.x,cubeMax.y,cubeMin.z),
		glm::vec3(cubeMax.x,cubeMax.y,cubeMin.z),

		// Top
		glm::vec3(cubeMin.x,cubeMax.y,cubeMax.z),
		glm::vec3(cubeMax.x,cubeMax.y,cubeMax.z),
		glm::vec3(cubeMax.x,cubeMax.y,cubeMin.z),
		glm::vec3(cubeMin.x,cubeMax.y,cubeMin.z),

		// Bottom
		glm::vec3(cubeMin.x,cubeMin.y,cubeMin.z),
		glm::vec3(cubeMax.x,cubeMin.y,cubeMin.z),
		glm::vec3(cubeMax.x,cubeMin.y,cubeMax.z),
		glm::vec3(cubeMin.x,cubeMin.y,cubeMax.z),

		// Left
		glm::vec3(cubeMin.x,cubeMin.y,cubeMin.z),
		glm::vec3(cubeMin.x,cubeMin.y,cubeMax.z),
		glm::vec3(cubeMin.x,cubeMax.y,cubeMax.z),
		glm::vec3(cubeMin.x,cubeMax.y,cubeMin.z),

		// Right
		glm::vec3(cubeMax.x,cubeMin.y,cubeMax.z),
		glm::vec3(cubeMax.x,cubeMin.y,cubeMin.z),
		glm::vec3(cubeMax.x,cubeMax.y,cubeMin.z),
		glm::vec3(cubeMax.x,cubeMax.y,cubeMax.z)
	};

	// Specify normals
	normals = {
		// Front
		glm::vec3(0,0,1),
		glm::vec3(0,0,1),
		glm::vec3(0,0,1),
		glm::vec3(0,0,1),

		// Back			
		glm::vec3(0,0,-1),
		glm::vec3(0,0,-1),
		glm::vec3(0,0,-1),
		glm::vec3(0,0,-1),

		// Top
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),
		glm::vec3(0,1,0),

		// Bottom
		glm::vec3(0,-1,0),
		glm::vec3(0,-1,0),
		glm::vec3(0,-1,0),
		glm::vec3(0,-1,0),

		// Left
		glm::vec3(-1,0,0),
		glm::vec3(-1,0,0),
		glm::vec3(-1,0,0),
		glm::vec3(-1,0,0),

		// Right
		glm::vec3(1,0,0),
		glm::vec3(1,0,0),
		glm::vec3(1,0,0),
		glm::vec3(1,0,0)
	};
	
	// Specify indices
	indices = {
		0,1,2,		0,2,3,			// Front
		4,5,6,		4,6,7,			// Back
		8,9,10,		8,10,11,		// Top
		12,13,14,	12,14,15,		// Bottom
		16,17,18,	16,18,19,		// Left
		20,21,22,	20,22,23,		// Right
	};


	if (isClient)
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
}

////////////////////////////////////////////////////////////////////////////////
/*
 * Cube destructor. Frees up memory associated with OpenGL variables.
 *
 * @author Part of 169 starter code
 */
Cube::~Cube()
{
	// Causes error?
	//if (isClient)
	//{
	//	cout << "deleting gl stuff" << endl;
	//	// Delete the VBOs and the VAO.
	//	glDeleteBuffers(1, &VBO_positions);
	//	cout << "1" << endl;
	//	glDeleteBuffers(1, &VBO_normals);
	//	cout << "2" << endl;
	//	glDeleteBuffers(1, &EBO);
	//	cout << "3" << endl;
	//	glDeleteVertexArrays(1, &VAO);
	//}

	boundingBox->setActive(false);
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
void Cube::draw(const glm::mat4& viewProjMtx, GLuint shader)
{
	if (isClient)
	{
		// activate the shader program 
		glUseProgram(shader);

		// get the locations and send the uniforms to the shader 
		glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
		glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

		// Bind the VAO
		glBindVertexArray(VAO);

		// draw the points using triangles, indexed with the EBO
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		// Unbind the VAO and shader program
		glBindVertexArray(0);
		glUseProgram(0);

		if (Window::debugMode) {
			boundingBox->draw(viewProjMtx, shader);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

/*
 * Called every frame from idle callback. Use this method to perform any operations
 * on the cube which need to happen every frame.
 *
 * @author Part of 169 starter code
 */
void Cube::update()
{

}

/*
 * Returns the model matrix of the cube.
 *
 * @return model matrix of cube
 * @author Lucas Hwang
 */
glm::mat4 Cube::getModel() {
	return model;
}

/*
 * Updates the model matrix of the cube.
 *
 * @param m Updated model matrix
 * @author Lucas Hwang
 */
void Cube::setModel(glm::mat4 m) {
	model = m;
	boundingBox->setModel(m);
}

/*
 * Updates the color of the cube.
 *
 * @param c Updated color matrix
 * @author Lucas Hwang
 */
void Cube::setColor(glm::vec3 c) {
	color = c;
}

/*
 * Returns the color of the cube.
 *
 * @return color of cube
 * @author Lucas Hwang
 */
glm::vec3 Cube::getColor() {
	return color;
}

////////////////////////////////////////////////////////////////////////////////
