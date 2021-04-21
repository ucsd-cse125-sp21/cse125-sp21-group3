#include "Model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/gtx/string_cast.hpp>
#include "AssimpToGlmHelper.h"
#define AI_MATKEY_GLTF_MATNAME "?mat.name", 0, 0


Model::Model(string const& path, glm::mat4 _rootModel) 
{
    rootModel = _rootModel;
    gammaCorrection = false;
    loadModel(path);
    animationPlayer = new AnimationPlayer();
}

// draws the model, and thus all its meshes
void Model::draw(const glm::mat4& viewProjMtx, GLuint shader)
{
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

    processAnimations(scene);
}

void Model::processAnimations(const aiScene* scene) {

    for (int i = 0; i < scene->mNumAnimations; i++) {
        aiAnimation* animation = scene->mAnimations[i];
        //cout << "animation name: " << animation->mName.C_Str() << endl;
        vector<aiNodeAnim*> animNodeList;
        for (int j = 0; j < animation->mNumChannels; j++) {
            aiNodeAnim* animNode = animation->mChannels[j];
            animNodeList.push_back(animNode);
            //cout << "node name: " << node->mNodeName.C_Str() << endl;
        }
        animationPlayer->animationClipList.push_back(new AnimationClip(animNodeList, meshes));
    }
}

void Model::processNode(aiNode* node, const aiScene* scene, glm::mat4 rootTransform)
{
    //cout << "processing node: " << node->mName.C_Str() << endl;
    //cout << "numMeshes: " << node->mNumMeshes << endl;
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        glm::mat4 localTransform = AssimpToGlmHelper::convertAiMat4ToGlmMat4(node->mTransformation);
        glm::mat4 model = rootTransform * localTransform;
        meshes.push_back(processMesh(mesh, scene, model));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, rootTransform);  
    }

}


Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 model)
{
    //cout << "mesh: " << mesh->mName.C_Str() << endl;
    // data to fill
    vector<glm::vec3> positions;
    vector<glm::vec3> normals;
    vector<unsigned int> indices;
    
   
    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        positions.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
        // normals
        if (mesh->HasNormals())
        {
            normals.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
        }
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps

    aiColor4D color(0.0f, 0.0f, 0.0f, 1.0f);
    material->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR, color);

    // return a mesh object created from the extracted mesh data
    Mesh* m = new Mesh(positions, normals, indices);
    m->model = model;
    m->baseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //m->baseColor = glm::vec4(color.r, color.g, color.b, color.a);
    m->name = mesh->mName.C_Str();
    return m;
}