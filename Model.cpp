#include "Model.h"
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
#include <glm/gtx/string_cast.hpp>
#include "AssimpToGlmHelper.h"
#define AI_MATKEY_GLTF_MATNAME "?mat.name", 0, 0


Model::Model(string const& path, glm::mat4 _rootModel) 
{
    rootModel = _rootModel;
    gammaCorrection = false;
    animationPlayer = new AnimationPlayer();
    meshCounter = 0;
    loadModel(path);
}

// draws the model, and thus all its meshes
void Model::draw(const glm::mat4& viewProjMtx, GLuint shader)
{
    //cout << "meshes.size: " << meshes.size() << endl;
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i]->draw(viewProjMtx, shader);
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
    processNode(scene->mRootNode, scene, rootModel);
    processBones(scene);
    processAnimations(scene);
}

void Model::processAnimations(const aiScene* scene) {

    for (int i = 0; i < scene->mNumAnimations; i++) {
        aiAnimation* animation = scene->mAnimations[i];
        cout << "animation: " << animation->mName.C_Str() << endl;
        vector<AnimationNode*> animNodeList;
        for (int j = 0; j < animation->mNumChannels; j++) {
            aiNodeAnim* aiNodeAnim = animation->mChannels[j];
            animNodeList.push_back(new AnimationNode(aiNodeAnim, meshes, animation->mTicksPerSecond));
        }
        animationPlayer->animationClipList.push_back(new AnimationClip(animNodeList));
    }
}

void Model::processNode(aiNode* node, const aiScene* scene, glm::mat4 rootTransform)
{
    // process each mesh located at the current node
    glm::mat4 localTransform = AssimpToGlmHelper::convertAiMat4ToGlmMat4(node->mTransformation);
    glm::mat4 nodeModel = rootTransform * localTransform;
    nodeTransformMap.insert(pair<string, glm::mat4>(node->mName.C_Str(), nodeModel));
    
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, nodeModel));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        //processNode(node->mChildren[i], scene, rootTransform);  
        processNode(node->mChildren[i], scene, nodeModel);
    }
}

Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 model)
{
    // return a mesh object created from the extracted mesh data
    Mesh* m = new Mesh(mesh, scene, model);
    m->id = meshCounter;
    meshCounter++;
    return m;
}

void Model::processBones(const aiScene* scene) {

    
    for (int i = 0; i < meshes.size(); i++) {
        Mesh* mesh = meshes.at(i);
        vector<glm::vec3> bonePositions(mesh->positions.size());
        vector<glm::vec3> boneNormals(mesh->positions.size());
        for (int j = 0; j < mesh->bones.size(); j++) {
            Bone* bone = mesh->bones.at(j);
            //for loading in model using bones
            glm::mat4 nodeModel(1.0f);
            if (nodeTransformMap.find(bone->name) != nodeTransformMap.end()) {
                nodeModel = nodeTransformMap.find(bone->name)->second;
            }
            else {
                cout << "could not find nodeModel for: " << bone->name << endl;
            }
            
            for (int k = 0; k < bone->vertexIds.size(); k++) {
            	int vertexId = bone->vertexIds.at(k);
            	glm::vec4 v(mesh->positions.at(vertexId).x, mesh->positions.at(vertexId).y, mesh->positions.at(vertexId).z, 1.0f);
            	glm::vec4 v_prime = nodeModel * bone->offsetMatrix * v;
            	bonePositions.at(vertexId) += glm::vec3(v_prime.x, v_prime.y, v_prime.z) * bone->weights.at(k);

            	glm::vec4 n(mesh->normals.at(vertexId).x, mesh->normals.at(vertexId).y, mesh->normals.at(vertexId).z, 0.0f);
            	glm::vec4 n_prime = nodeModel * bone->offsetMatrix * n;
            	boneNormals.at(vertexId) += glm::vec3(n_prime.x, n_prime.y, n_prime.z) * bone->weights.at(k);
            }
        }

        if (mesh->bones.size() > 0) {
            mesh->positions = bonePositions;
            mesh->normals = boneNormals;
            glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_positions);
            glBufferSubData(GL_ARRAY_BUFFER, 0.0f, sizeof(glm::vec3) * mesh->positions.size(), mesh->positions.data());
            glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_normals);
            glBufferSubData(GL_ARRAY_BUFFER, 0.0f, sizeof(glm::vec3) * mesh->normals.size(), mesh->normals.data());
        }
    }
}