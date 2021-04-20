#ifndef MESH_H
#define MESH_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include <string>
#include <vector>
using namespace std;

class Mesh {
public:
    // mesh Data
    //vector<Vertex>       vertices;
    //vector<unsigned int> indices;
    //vector<Texture>      textures;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    glm::mat4 model;
    glm::vec4 baseColor;
    
    string name;

    // constructor
    Mesh(vector<glm::vec3> _positions, vector<glm::vec3> _normals, vector<unsigned int> _indices);
    
    // render the mesh
    void draw(const glm::mat4& viewProjMtx, GLuint shader);
    

private:
    // render data 
    GLuint VAO;
    GLuint VBO_positions, VBO_normals, EBO;
    
    // initializes all the buffer objects/arrays
    void setupMesh();
};
#endif