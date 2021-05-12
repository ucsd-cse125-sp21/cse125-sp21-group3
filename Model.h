#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>
#include "AnimationClip.h"
#include "Mesh.h"
#include "Node.h"
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
    glm::mat4 rootModel; //modify this to control position of model
    glm::mat4 animationRootModel; //this should not be modified

    vector<AnimationClip*> animationClipList;
    map<string, glm::mat4> boneTransformMap;
    Node* root;

    // constructor, expects a filepath to a 3D model.
    Model(string const& path, glm::mat4 _rootModel);

    // draws the model, and thus all its meshes
    void draw(const glm::mat4& viewProjMtx, GLuint shader);
    void update();
    void updateNodes(Node* node, glm::mat4 parentTransform);
    void playAnimation(AnimationClip* animationClip, float time, bool reverse);
    void rotateAnimation(float amount, glm::vec3 p); //rotates animationRootModel around point p
    void rotate(float amount, glm::vec3 p); //rotates rootModel around point p
private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* aiNode, Node* node, const aiScene* scene, glm::mat4 rootTransform);

    void processAnimations(const aiScene* scene);

    Mesh* processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 model);
    
};
#endif