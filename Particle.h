#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "core.h"

////////////////////////////////////////////////////////////////////////////////
/*
 * File Name: Particle.h
 *
 * Contains Particle class and defines all the properties of a particle and methods that
 * can be performed on a particle.
 *
 * @author Lucas Hwang
 */
class Particle
{
public:
	GLuint VAO;
	GLuint VBO_positions, VBO_normals, EBO;

	glm::vec3 velocity;
	glm::vec3 position;
	glm::mat4 model;
	glm::vec3 color;
	float spawnTime;
	float lifetime;
	float lifespan;
	//Particle Information
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
	Particle(glm::vec3 _position, glm::vec3 _velocity, float time, glm::vec3 _color);
	~Particle();

	void draw(const glm::mat4& viewProjMtx, GLuint shader);
	void update(float deltaTime);
};

////////////////////////////////////////////////////////////////////////////////

#endif
