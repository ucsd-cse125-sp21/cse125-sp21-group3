#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>

#include "Mesh.h"
#include "AnimationPlayer.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

class Model
{
public:
    // model data 
    vector<Mesh*> meshes;
    int meshCounter;

    string directory;
    bool gammaCorrection;
    glm::mat4 rootModel;
    AnimationPlayer* animationPlayer;
    map<string, glm::mat4> nodeTransformMap;
    // constructor, expects a filepath to a 3D model.
    Model(string const& path, glm::mat4 _rootModel);

    // draws the model, and thus all its meshes
    void draw(const glm::mat4& viewProjMtx, GLuint shader);

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene, glm::mat4 rootTransform);
    
    void processBones(const aiScene* scene);

    void processAnimations(const aiScene* scene);

    Mesh* processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 model);
};
#endif