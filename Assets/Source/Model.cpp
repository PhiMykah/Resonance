#include "Model.h"

/*
Constructor for Model.
takes in gltf file as argument

Parameters
----------
file : const char *
    Target gltf file that defines model

Returns
-------
Model Object
*/
Model::Model(const char* file){

    std::string text = get_file_contents(file); // Load file into string object
    
    Model::JSON = json::parse(text); // Parse the data into a JSON format

    Model::file = file; // Set file path as target file for model

    data = getData(); // Obtain model data and allocate to bytes

    // Start node traversal and obtain data for meshes
    traverseNode(0);
}

/*
Draw the entire model by drawing each mesh

Parameters
----------
shader : Shader&
    Target shader used for drawing
camera : Camera&
    Target viewport object will be drawn in

Returns
-------
None
*/
void Model::Draw(Shader& shader, Camera & camera){
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].Mesh::Draw(shader, camera, meshMatrices[i], meshTranslations[i], meshRotations[i], meshScales[i]);
    }
}

/*
Loads mesh to model based on json index

Parameters
----------
indMesh : unsigned int
    Index of mesh in JSON meshes dictionary

Returns
-------
None
*/
void Model::loadMesh(unsigned int indMesh){

    // Position accessor indices
    unsigned int posAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["POSITION"];
    // Normal accessor indices
    unsigned int normalAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["NORMAL"];
    // texture accesor indices
    unsigned int texAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
    // index accessor indices
    unsigned int indAccInd = JSON["meshes"][indMesh]["primitives"][0]["indices"];

    // Obtain the position values and group the position values into vec3
    std::vector<float> positionVector = getFloats(JSON["accessors"][posAccInd]);
    std::vector<glm::vec3> positions = groupFloatsVec3(positionVector);

    // Obtain the normal values and group the normal values into vec3
    std::vector<float> normalVector = getFloats(JSON["accessors"][normalAccInd]);
    std::vector<glm::vec3> normals = groupFloatsVec3(normalVector);

    // Obtain the tex values and group the tex values into vec3
    std::vector<float> texVector = getFloats(JSON["accessors"][texAccInd]);
    std::vector<glm::vec2> texUVs = groupFloatsVec2(texVector);

    // Assemble positions normals and texture UVs into vertices
    Vertices vertices = assembleVertices(positions, normals, texUVs);
    // Obtain indices
    Indices indices = getIndices(JSON["accessors"][indAccInd]);
    // Collect textures
    Textures textures = getTextures();
    
    // Create mesh from vertices, indices, and textures, then add to mesh container
    meshes.push_back(Mesh(vertices, indices, textures));
}

/*
Recursively traverses through gltf json nodes 
and loads each mesh

Parameters
----------
nextNode : unsigned int
    Next node in json tree

matrix : glm::mat4
    Current model matrix

Returns
-------
None
*/
void Model::traverseNode(unsigned int nextNode, glm::mat4 matrix)
{
	// Current node
	json node = JSON["nodes"][nextNode];

	// Get translation if it exists
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	if (node.find("translation") != node.end())
	{
		float transValues[3];
		for (unsigned int i = 0; i < node["translation"].size(); i++)
			transValues[i] = (node["translation"][i]);
		translation = glm::make_vec3(transValues);
	}
	// Get quaternion if it exists
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	if (node.find("rotation") != node.end())
	{
		float rotValues[4] =
		{ // Rotation values are loaded in this order because of the way gltf stores quaternions
			node["rotation"][3],
			node["rotation"][0],
			node["rotation"][1],
			node["rotation"][2]
		};
		rotation = glm::make_quat(rotValues);
	}
	// Get scale if it exists
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	if (node.find("scale") != node.end())
	{
		float scaleValues[3];
		for (unsigned int i = 0; i < node["scale"].size(); i++)
			scaleValues[i] = (node["scale"][i]);
		scale = glm::make_vec3(scaleValues);
	}
	// Get matrix if it exists
	glm::mat4 matNode = glm::mat4(1.0f);
	if (node.find("matrix") != node.end())
	{
		float matValues[16];
		for (unsigned int i = 0; i < node["matrix"].size(); i++)
			matValues[i] = (node["matrix"][i]);
		matNode = glm::make_mat4(matValues);
	}

	// Initialize matrices
	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 sca = glm::mat4(1.0f);

	// Use translation, rotation, and scale to change the initialized matrices
	trans = glm::translate(trans, translation);
	rot = glm::mat4_cast(rotation);
	sca = glm::scale(sca, scale);

	// Multiply all matrices together
	glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;

	// Check if the node contains a mesh and if it does load it
	if (node.find("mesh") != node.end())
	{
		meshTranslations.push_back(translation);
		meshRotations.push_back(rotation);
		meshScales.push_back(scale);
		meshMatrices.push_back(matNextNode);

		loadMesh(node["mesh"]);
	}

	// Check if the node has children, and if it does, apply this function to them with the matNextNode
	if (node.find("children") != node.end())
	{
		for (unsigned int i = 0; i < node["children"].size(); i++)
			traverseNode(node["children"][i], matNextNode);
	}
}

/*
Obtain the json data from file 
and store into byte vector

Parameters
----------
None

Returns
-------
data : std::vector<unsigned char>
    Bytes vector containing json data
*/
std::vector<unsigned char> Model::getData(){

    std::string bytesText; // Hold raw text of file
    std::string uri = JSON["buffers"][0]["uri"]; // Collect the location of the data from buffer (bin file)

    // Obtain file parent directory and link to URI obtain raw text data
    std::string fileStr = std::string(file);
    std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
    bytesText = get_file_contents((fileDirectory + uri).c_str());

    // Set data vector object to link to start and end of raw text stream
    // Represent raw text of file as bytes, and return
    std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
    return data;
}

/*
Get floats from json accessor
(e.g. coordinates, normals, color)

Parameters
----------
accessor : json
    Json-formatted tree for target accessor

Returns
-------
floatVec : std::vector<float>
    Vector of floats from accessor
*/
std::vector<float> Model::getFloats(json accessor){

    std::vector<float> floatVec;

    // Obtain buffer view, count, byte offset, and type from accessor
    // Value function gives value if it exists, and the second parameter otherwise

    unsigned int buffViewInd = accessor.value("bufferView", 1); // Points to buffer view
    unsigned int count = accessor["count"]; // Number of floats to obtain
    // Index to start looking at the data with respect to the buffer view
    unsigned int accByteOffset = accessor.value("byteOffset", 0);
    std::string type = accessor["type"]; // Float grouping format (e.g. SCALAR, VEC2, VEC3, VEC4)

    // Obtain buffer view dictionary from the buffer view index
    json bufferView = JSON["bufferViews"][buffViewInd];

    // Index for where the buffer view begins.
    // This is different than the accessor byte offset since
    // two different accessors may share the same buffer view 
    unsigned int byteOffset = bufferView.value("byteOffset", 0); 

    // Set number of float per vertex based on accessor data type
    unsigned int floatPerVertex;    
    if (type == "SCALAR") floatPerVertex = 1;
    else if (type == "VEC2") floatPerVertex = 2;
    else if (type == "VEC3") floatPerVertex = 3;
    else if (type == "VEC4") floatPerVertex = 4;
    else throw std::invalid_argument("Vertex Accessor type is invalid (not SCALAR, VEC2, VEC3, or VEC4)");

    // Calculate start of data index and data size

    unsigned int dataBegin = byteOffset + accByteOffset; // Starting index of the data
    unsigned int dataLength = count * 4 * floatPerVertex; // Length of data in bits

    // Loop over data and store information in temporary array.
    // Then convert byte data to float values, and store in vector
    for (unsigned int i = dataBegin; i < dataBegin + dataLength;){
        unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++]};
        float value;
        // Convert bytes data to float 
        std::memcpy(&value, bytes, sizeof(float));
        floatVec.push_back(value);
    }

    return floatVec;
}

/*
Get vertex indices from json accessor

Parameters
----------
accessor : json
    Json-formatted tree for target accessor

Returns
-------
indices : Indices
    Vector of GL unsigned ints from accessor
*/
Indices Model::getIndices(json accessor){

    Indices indices;

    // Obtain buffer view, count, byte offset, and type from accessor
    // Value function gives value if it exists, and the second parameter otherwise

    unsigned int buffViewInd = accessor.value("bufferView", 0); // Points to buffer view
    unsigned int count = accessor["count"]; // Number of floats to obtain
    // Index to start looking at the data with respect to the buffer view
    unsigned int accByteOffset = accessor.value("byteOffset", 0);
    unsigned int componentType = accessor["componentType"]; // Datatype that is used to store indices

    // Obtain buffer view dictionary from the buffer view index
    json bufferView = JSON["bufferViews"][buffViewInd];

    // Index for where the buffer view begins.
    // This is different than the accessor byte offset since
    // two different accessors may share the same buffer view 
    unsigned int byteOffset = bufferView.value("byteOffset", 0); 

    // Calculate start of data index and data size

    unsigned int dataBegin = byteOffset + accByteOffset; // Starting index of the data

    // Store data based on the index datatype into indices vector
    switch (componentType)
    {
    case 5125: // Datatype is unsigned int
        // Loop over data and store information in temporary array.
        // Then convert byte data to unsigned int values, and store in vector as GLuint
        for (unsigned int i = dataBegin; i < byteOffset + accByteOffset + count * 4;)
        {
            unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++]};
            unsigned int value;
            // Convert bytes data to unsigned int
            std::memcpy(&value, bytes, sizeof(unsigned int));
            indices.push_back((GLuint)value);
        }
        break;
    case 5123: // Datatype is unsigned short
        // Loop over data and store information in temporary array.
        // Then convert byte data to unsigned short values, and store in vector as GLuint
        for (unsigned int i = dataBegin; i < byteOffset + accByteOffset + count * 2;)
        {
            unsigned char bytes[] = { data[i++], data[i++] };
            unsigned short value;
            // Convert bytes data to unsigned short
            std::memcpy(&value, bytes, sizeof(unsigned short));
            indices.push_back((GLuint)value);
        }
        break;
    case 5122: // Datatype is short
        // Loop over data and store information in temporary array.
        // Then convert byte data to short values, and store in vector as GLuint
        for (unsigned int i = dataBegin; i < byteOffset + accByteOffset + count * 2;)
        {
            unsigned char bytes[] = { data[i++], data[i++] };
            short value;
            // Convert bytes data to unsigned short
            std::memcpy(&value, bytes, sizeof(short));
            indices.push_back((GLuint)value);
        }
        break;
    default:
        throw std::invalid_argument("Index Accessor type is invalid (not unsigned int, unsigned short, or short)");
        break;
    }

    return indices;
}

/*
Obtain image files from json and store in vector

Parameters
----------
None

Returns
-------
textures : Textures
    Vertex containing loaded textures
*/
Textures Model::getTextures(){
    std::vector<Texture> textures;

    // Obtain parent directory of file

	std::string fileStr = std::string(file);
	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

	// Iterate over all of the images contained by the JSON.
    // Keep track of the texture unit during iteration.
    // Obtain texture type based on name of the texture.
    // ************* NOTE **************
    // This method of texture checking is not ideal, and may vary from model to model
	for (unsigned int i = 0; i < JSON["images"].size(); i++)
	{
		// uri of current texture
		std::string texPath = JSON["images"][i]["uri"];

		// Test if texture already exists and is already loaded
        // Skip loading if texture exists currently
		bool textureExists = false;
		for (unsigned int j = 0; j < loadedTexNames.size(); j++)
		{
			if (loadedTexNames[j] == texPath)
			{
				textures.push_back(loadedTex[j]);
				textureExists = true;
				break;
			}
		}

		// If the texture has been loaded, skip this
		if (!textureExists)
		{
			// Determine the type of texture based on the name of the texture file.
            // `baseColor` is diffuse, `metallicRoughness` is roughness but may be used as specular. 
			if (texPath.find("baseColor") != std::string::npos || texPath.find("diffuse") != std::string::npos)
			{
				Texture diffuse = Texture((fileDirectory + texPath).c_str(), "diffuse", loadedTex.size(), GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);
				textures.push_back(diffuse);
				loadedTex.push_back(diffuse);
				loadedTexNames.push_back(texPath);
			}
			// Load specular texture
			else if (texPath.find("metallicRoughness") != std::string::npos || texPath.find("specular") != std::string::npos)
			{
                // This does not work fully as specular since metallicRoughness is meant for physically based rendering
				Texture specular = Texture((fileDirectory + texPath).c_str(), "specular", loadedTex.size(), GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST);
				textures.push_back(specular);
				loadedTex.push_back(specular);
				loadedTexNames.push_back(texPath);
			}
		}
	}

	return textures;
}
 
/*
Store each position, normal, and textUV triplet into 
a vertex

Parameters
----------
positions : std::vector<glm::vec3> 
    Mesh's coordinates
normals : std::vector<glm::vec3>
    Mesh's normals
texUVs : std::vector<glm::vec2>
    Mesh's texUVs

Returns
-------
    Vector containing mesh vertices
*/
Vertices Model::assembleVertices
(
    std::vector<glm::vec3> positions,
    std::vector<glm::vec3> normals,
    std::vector<glm::vec2> texUVs
){
    Vertices vertices;
    // Iterate over all vectors to create vertices
    // (Positions is chosen arbitrarily since they should all be of equal size)
    for (unsigned int i = 0; i < positions.size(); i++)
    {
        vertices.push_back
        (
            Vertex
            {
                positions[i],
                normals[i],
                glm::vec3(1.0f, 1.0f, 1.0f),
                texUVs[i]
            }
        );
    }
    
    return vertices;
}

/*
Group every two floats in vector into glm vec2 pairs

Parameters
----------
std::vector<float> floatVec
    Ungrouped floats
Returns
-------
std::vector<glm::vec2> 
    Grouped floats
*/
std::vector<glm::vec2> Model::groupFloatsVec2(std::vector<float> floatVec){

    std::vector<glm::vec2> vectors;
    for (unsigned int i = 0; i < floatVec.size();){
        vectors.push_back(glm::vec2(floatVec[i++], floatVec[i++]));
    }
    return vectors;
}

/*
Group every three floats in vector into glm vec3 pairs

Parameters
----------
std::vector<float> floatVec
    Ungrouped floats
Returns
-------
std::vector<glm::vec3> 
    Grouped floats
*/
std::vector<glm::vec3> Model::groupFloatsVec3(std::vector<float> floatVec){

    std::vector<glm::vec3> vectors;
    for (unsigned int i = 0; i < floatVec.size();){
        vectors.push_back(glm::vec3(floatVec[i++], floatVec[i++], floatVec[i++]));
    }
    return vectors;
}

/*
Group every four floats in vector into glm vec4 pairs

Parameters
----------
std::vector<float> floatVec
    Ungrouped floats
Returns
-------
std::vector<glm::vec4> 
    Grouped floats
*/
std::vector<glm::vec4> Model::groupFloatsVec4(std::vector<float> floatVec){

    std::vector<glm::vec4> vectors;
    for (unsigned int i = 0; i < floatVec.size();){
        vectors.push_back(glm::vec4(floatVec[i++], floatVec[i++], floatVec[i++], floatVec[i++]));
    }
    return vectors;
}