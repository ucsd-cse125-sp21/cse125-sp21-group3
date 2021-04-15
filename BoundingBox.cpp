#include "BoundingBox.h"
#include "glm/gtx/euler_angles.hpp"

/*
 * File Name: BoundingBox.cpp
 *
 * This file contains the BoundingBox implementation. This class will check
 * for collisions. Collision response will be handled by objects.
 *
 * @author Lucas Hwang
 */

 ////////////////////////////////////////////////////////////////////////////////

/*
 * Default constructor for the camera.
 *
 * @return Player object
 * @author Lucas Hwang
 */
BoundingBox::BoundingBox(glm::vec3 boxMin, glm::vec3 boxMax) {
   
    min = boxMin;
    max = boxMax;
    active = true;

	color = glm::vec3(0.0f, 1.0f, 0.0f);
	model = glm::mat4(1.0f);

	// Specify vertex positions
	positions = {
		// Front
		glm::vec3(min.x,min.y,max.z),
		glm::vec3(max.x,min.y,max.z),
		glm::vec3(max.x,max.y,max.z),
		glm::vec3(min.x,max.y,max.z),

		// Back
		glm::vec3(max.x,min.y,min.z),
		glm::vec3(min.x,min.y,min.z),
		glm::vec3(min.x,max.y,min.z),
		glm::vec3(max.x,max.y,min.z),

		// Top
		glm::vec3(min.x,max.y,max.z),
		glm::vec3(max.x,max.y,max.z),
		glm::vec3(max.x,max.y,min.z),
		glm::vec3(min.x,max.y,min.z),

		// Bottom
		glm::vec3(min.x,min.y,min.z),
		glm::vec3(max.x,min.y,min.z),
		glm::vec3(max.x,min.y,max.z),
		glm::vec3(min.x,min.y,max.z),

		// Left
		glm::vec3(min.x,min.y,min.z),
		glm::vec3(min.x,min.y,max.z),
		glm::vec3(min.x,max.y,max.z),
		glm::vec3(min.x,max.y,min.z),

		// Right
		glm::vec3(max.x,min.y,max.z),
		glm::vec3(max.x,min.y,min.z),
		glm::vec3(max.x,max.y,min.z),
		glm::vec3(max.x,max.y,max.z)
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


/*
 * Checks if the bounding box of this object is colliding with another
 * bounding box b.
 *
 * @param b Bounding box of the object we are checking for a collision with
 * @return Returns true if there is a collision and false if not
 * @author Lucas Hwang
 */
bool BoundingBox::checkCollision(BoundingBox* b) {

    //checking for collision with min point of b
    glm::vec3 a_min = min;
    glm::vec3 a_max = max;
    glm::vec3 b_min = b->getMin();
    glm::vec3 b_max = b->getMax();
    return (a_min.x <= b_max.x && a_max.x >= b_min.x) &&
        (a_min.y <= b_max.y && a_max.y >= b_min.y) &&
        (a_min.z <= b_max.z && a_max.z >= b_min.z);
}

void BoundingBox::draw(const glm::mat4& viewProjMtx, GLuint shader) {
	
	// activate the shader program 
	glUseProgram(shader);

	// get the locations and send the uniforms to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
	glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

	// Bind the VAO
	glBindVertexArray(VAO);

	// draw the points using triangles, indexed with the EBO
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

void BoundingBox::update(glm::vec3 _min, glm::vec3 _max) {

	min = _min;
	max = _max;

	// Specify vertex positions
	positions = {
		// Front
		glm::vec3(min.x,min.y,max.z),
		glm::vec3(max.x,min.y,max.z),
		glm::vec3(max.x,max.y,max.z),
		glm::vec3(min.x,max.y,max.z),

		// Back
		glm::vec3(max.x,min.y,min.z),
		glm::vec3(min.x,min.y,min.z),
		glm::vec3(min.x,max.y,min.z),
		glm::vec3(max.x,max.y,min.z),

		// Top
		glm::vec3(min.x,max.y,max.z),
		glm::vec3(max.x,max.y,max.z),
		glm::vec3(max.x,max.y,min.z),
		glm::vec3(min.x,max.y,min.z),

		// Bottom
		glm::vec3(min.x,min.y,min.z),
		glm::vec3(max.x,min.y,min.z),
		glm::vec3(max.x,min.y,max.z),
		glm::vec3(min.x,min.y,max.z),

		// Left
		glm::vec3(min.x,min.y,min.z),
		glm::vec3(min.x,min.y,max.z),
		glm::vec3(min.x,max.y,max.z),
		glm::vec3(min.x,max.y,min.z),

		// Right
		glm::vec3(max.x,min.y,max.z),
		glm::vec3(max.x,min.y,min.z),
		glm::vec3(max.x,max.y,min.z),
		glm::vec3(max.x,max.y,max.z)
	};

	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferSubData(GL_ARRAY_BUFFER, 0.0f, sizeof(glm::vec3) * positions.size(), positions.data());
}
////////////////////////////////////////////////////////////////////////////////
