#ifndef MESH_H
#define MESH_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "core.h"
#include <string>
#include <vector>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/pbrmaterial.h>
#include "Bone.h"
using namespace std;

class Mesh {
public:
    // mesh Data
    //vector<Vertex>       vertices;
    //vector<unsigned int> indices;
    //vector<Texture>      textures;
    int id;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    std::vector<Bone*> bones;
    glm::mat4 model;
    glm::vec4 baseColor;
    
    string name;

    // constructor
    Mesh(aiMesh* aiMesh, const aiScene* scene, glm::mat4 _model);
    
    // render the mesh
    void draw(const glm::mat4& viewProjMtx, GLuint shader);
    void animateBone(glm::mat4 transform, Bone* bone);

    // render data 
    GLuint VAO;
    GLuint VBO_positions, VBO_normals, EBO;
    
    // initializes all the buffer objects/arrays
    void setupMesh();
};
#endif