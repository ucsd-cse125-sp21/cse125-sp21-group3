#include "Model.h"
#include "Window.h"
#include <glm/gtx/string_cast.hpp>
#include "AssimpToGlmHelper.h"
#define AI_MATKEY_GLTF_MATNAME "?mat.name", 0, 0


Model::Model(string const& path, glm::mat4 _rootModel, bool client) 
{
    rootModel = _rootModel;
    boundingBox == NULL;
    animationRootModel = rootModel;
    rootModelRotation = 0.0f;
    animationRootModelRotation = 0.0f;
    gammaCorrection = false;
    meshCounter = 0;
    isClient = client;

    if (isClient) {
        loadModel(path);
    }
    //only for chests
    opening = false;
}

Model::~Model()
{
    std::cout << "Deleting" << std::endl;
    if (boundingBox)
    {
        boundingBox->setActive(false);
    }
}

// draws the model, and thus all its meshes
void Model::draw(const glm::mat4& viewProjMtx, GLuint shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i]->draw(viewProjMtx, shader);

    if (Window::debugMode) {
        if (boundingBox != NULL) {
            boundingBox->draw(viewProjMtx, shader);
        }
    }
}

void Model::loadModel(string const& path)
{
    // read file via ASSIMP
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    
    root = new Node(scene->mRootNode->mName.C_Str());
    processNode(scene->mRootNode, root, scene, rootModel);
    processAnimations(scene);
}

void Model::processNode(aiNode* aiNode, Node* node, const aiScene* scene, glm::mat4 parentTransform)
{
    // process each mesh located at the current node
    glm::mat4 localTransform = AssimpToGlmHelper::convertAiMat4ToGlmMat4(aiNode->mTransformation);
    node->transform = localTransform;
    glm::mat4 nodeModel = parentTransform * localTransform;
    for (unsigned int i = 0; i < aiNode->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[aiNode->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, nodeModel)); //adding meshes to mesh master list in model
        node->meshList.push_back(meshes.at(meshes.size() - 1)); //adding node specific meshes to node
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < aiNode->mNumChildren; i++)
    {
        Node* child = new Node(aiNode->mChildren[i]->mName.C_Str());
        node->children.push_back(child);
        processNode(aiNode->mChildren[i], child, scene, nodeModel);
    }
}

Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 model)
{
    // return a mesh object created from the extracted mesh data
    Mesh* m = new Mesh(mesh, scene, model, isClient);
    m->id = meshCounter;
    meshCounter++;
    return m;
}

void Model::processAnimations(const aiScene* scene) {

    for (int i = 0; i < scene->mNumAnimations; i++) {
        aiAnimation* animation = scene->mAnimations[i];
        map<string, AnimationNode*> animationNodeMap;
        for (int j = 0; j < animation->mNumChannels; j++) {
            aiNodeAnim* aiNodeAnim = animation->mChannels[j];
            if (animationNodeMap.find(aiNodeAnim->mNodeName.C_Str()) != animationNodeMap.end()) {
                cout << "error: duplicate animation node names" << endl;
            }
            //cout << "animationNode: " << aiNodeAnim->mNodeName.C_Str() << endl;
            animationNodeMap.insert(pair<string, AnimationNode*>(aiNodeAnim->mNodeName.C_Str(), 
                new AnimationNode(aiNodeAnim, meshes, animation->mTicksPerSecond)));
        }
        animationClipList.push_back(new AnimationClip(animationNodeMap, meshes));
        //cout << "processAnimation: " << animation->mName.C_Str() << endl;
        animationClipList.at(animationClipList.size() - 1)->name = animation->mName.C_Str();
    }
}

void Model::update() {

    updateNodes(root, rootModel);
    
}

void Model::updateNodes(Node* node, glm::mat4 parentTransform)
{
    // process each mesh located at the current node

    glm::mat4 nodeModel = parentTransform * node->transform;
    for (int i = 0; i < node->meshList.size(); i++)
    {
        Mesh* mesh = node->meshList.at(i);
        mesh->model = nodeModel;
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (int i = 0; i < node->children.size(); i++)
    {
        Node* child = node->children.at(i);
        updateNodes(child, nodeModel);
    }
}

void Model::playAnimation(AnimationClip* animationClip, float speed, bool reverse) {

    float time;
    if (reverse) { 
        time = animationClip->prevTime - speed; 
    }
    else { 
        time = animationClip->prevTime + speed; 
    }
    if (time > animationClip->duration) {
        time = 0.0f;
    }
    if (time < 0.0f) {
        time = animationClip->duration;
    }
    
    animationClip->calculateBoneTransforms(time, root, animationRootModel);
    animationClip->applyBoneTransforms();
    animationClip->prevTime = time;
}

void Model::rotateAnimation(float amount, glm::vec3 p) {

    animationRootModelRotation += amount;
    /*if (animationRootModelRotation >= 2.0f * M_PI) {
        animationRootModelRotation -= 2.0f * M_PI;
    }*/
    glm::mat4 toOrigin = glm::translate(glm::mat4(1.0f), -p);
    glm::mat4 fromOrigin = glm::translate(glm::mat4(1.0f), p);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), amount, glm::vec3(0.0f, 1.0f, 0.0f));
    animationRootModel = fromOrigin * rotation * toOrigin * animationRootModel;   
}

void Model::rotate(float amount, glm::vec3 p) {

    rootModelRotation += amount;
    /*if (rootModelRotation >= 2.0f * M_PI) {
        rootModelRotation -= 2.0f * M_PI;
    }*/
    glm::mat4 toOrigin = glm::translate(glm::mat4(1.0f), -p);
    glm::mat4 fromOrigin = glm::translate(glm::mat4(1.0f), p);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), amount, glm::vec3(0.0f, 1.0f, 0.0f));
    rootModel = fromOrigin * rotation* toOrigin * rootModel;

}

void Model::rotateAnimation(glm::mat4 rotation, glm::vec3 p) {

    glm::mat4 toOrigin = glm::translate(glm::mat4(1.0f), -p);
    glm::mat4 fromOrigin = glm::translate(glm::mat4(1.0f), p);
    animationRootModel = fromOrigin * rotation * toOrigin * animationRootModel;
}

void Model::rotate(glm::mat4 rotation, glm::vec3 p) {

    glm::mat4 toOrigin = glm::translate(glm::mat4(1.0f), -p);
    glm::mat4 fromOrigin = glm::translate(glm::mat4(1.0f), p);
    rootModel = fromOrigin * rotation * toOrigin * rootModel;
}