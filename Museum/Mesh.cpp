#include "Mesh.h"
#include<limits>

Mesh::Mesh(const aiScene* scene, unsigned int meshIndex)
{
	const aiMesh* mesh = scene->mMeshes[meshIndex];
	name = mesh->mName.data;

	vec3 minBox(FLT_MAX), maxBox(-FLT_MAX);

	//EXTRACT vertices, normals, and uvs
	aiVector3D v;
	for (int t = 0; t < mesh->mNumVertices; ++t) {
		//Get vertices
		v = mesh->mVertices[t];
		vertexList.push_back(vec4(v.x, v.y, v.z, 1));
		//bounding box and box max
		maxBox.x = max(maxBox.x, v.x);
		minBox.x = min(minBox.x, v.x);
		minBox.y = min(minBox.y, v.y);
		maxBox.y = max(maxBox.y, v.y);
		maxBox.z = max(maxBox.z, v.z);
		minBox.z = min(minBox.z, v.z);

		//Get Normals
		v = mesh->mNormals[t];
		normalList.push_back(vec4(v.x, v.y, v.z, 0));

		//Get UVs
		//NEED TO REVISE THIS LATER: CURRENTLY WORKING WITH FIRST OF 8 SLOTS
		if (mesh->mTextureCoords[0]) {
			v = mesh->mTextureCoords[0][t];
			uvList.push_back(vec2(v.x, v.y));
		}
	}

	//Get indices
	for (int t = 0; t < mesh->mNumFaces; t++) {
		const aiFace* face = &mesh->mFaces[t];
		for (int i = 0; i < face->mNumIndices; i++) {
			indexList.push_back(face->mIndices[i]);
		}
	}

	BoxMax = max(max(maxBox.x - minBox.x,
		maxBox.y - minBox.y), maxBox.z - minBox.z);

	//8 Axis Align bounding box points
	boundBox.push_back(vec3(minBox.x, maxBox.y, minBox.z)); //back left top
	boundBox.push_back(vec3(minBox.x, minBox.y, minBox.z)); //back left down
	boundBox.push_back(vec3(maxBox.x, minBox.y, minBox.z)); //back right down
	boundBox.push_back(vec3(maxBox.x, maxBox.y, minBox.z)); //back right top
	boundBox.push_back(vec3(minBox.x, maxBox.y, maxBox.z)); //front left top
	boundBox.push_back(vec3(minBox.x, minBox.y, maxBox.z)); //front left down
	boundBox.push_back(vec3(maxBox.x, minBox.y, maxBox.z)); //front right down
	boundBox.push_back(vec3(maxBox.x, maxBox.y, maxBox.z)); //front right top

	centerBox = vec4((maxBox.x + minBox.x) / 2,
		(minBox.y + maxBox.y) / 2,
		(maxBox.z + minBox.z) / 2, 1.0);

	//EXTRACT materials
	const aiMaterial* meshMaterial = scene->mMaterials[mesh->mMaterialIndex];
	aiReturn success;
	aiColor4D ai4d;
	if (aiGetMaterialColor(meshMaterial, AI_MATKEY_COLOR_DIFFUSE, &ai4d) == AI_SUCCESS) {
		ambient = vec3(ai4d.r, ai4d.g, ai4d.b);
	}
	if (aiGetMaterialColor(meshMaterial, AI_MATKEY_COLOR_DIFFUSE, &ai4d) == AI_SUCCESS) {
		diffuse = vec3(ai4d.r, ai4d.g, ai4d.b);
	}
	if (aiGetMaterialColor(meshMaterial, AI_MATKEY_COLOR_SPECULAR, &ai4d) == AI_SUCCESS) {
		specular = vec3(ai4d.r, ai4d.g, ai4d.b);
	}
	if (aiGetMaterialFloat(meshMaterial, AI_MATKEY_SHININESS, &shininess) == AI_SUCCESS) {
	}
	if (aiGetMaterialColor(meshMaterial, AI_MATKEY_COLOR_EMISSIVE, &ai4d) == AI_SUCCESS) {
		emission = vec3(ai4d.r, ai4d.g, ai4d.b);
	}

	//EXTRACT texture name(s) and load
	textureAvailable = false; //assume no texture is available

	int texIndex = 0; //only get first texture associated with material
	aiReturn texFound;
	aiString path;	// filename
					//Get texture associated with this mesh's meterial
	texFound = meshMaterial->GetTexture(aiTextureType_DIFFUSE,
		texIndex, &path);

	//if texture was found, set tex file name
	if (texFound == AI_SUCCESS) {
		textureAvailable = true;
		textureFileName = path.data;
		textureFileName = textureFileName.substr(textureFileName.find_last_of('\\') + 1);
		textureFileName = ".\\textures\\" + textureFileName;
	}
}

bool Mesh::LoadGPU(GLuint program)
{
	this->program = program;
	//Generate and bind mesh's vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint position_loc, normal_loc, uv_loc;

	int numVertices = vertexList.size();
	int numNormals = normalList.size();
	int numUVs = uvList.size();
	int numIndices = indexList.size();

	//size of vertices, normals, uvs
	int sizeVertices = numVertices * sizeof(vec4);
	int sizeNormals = numNormals * sizeof(vec4);
	int sizeUVs = numUVs * sizeof(vec2);
	int sizeIndices = numIndices * sizeof(GLuint);

	cout << "\nMesh name: " << name << endl;
	cout << "Vertices: " << numVertices << endl;
	cout << "Normals: " << numNormals << endl;
	cout << "UVs: " << numUVs << endl;
	cout << "Indices: " << numIndices << endl;

	//Generate Array buffer
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	//Allocate buffer size enough to hold vertices, normals, and UVs
	glBufferData(GL_ARRAY_BUFFER, sizeVertices + sizeNormals + sizeUVs, NULL, GL_STATIC_DRAW);

	//Generate Index Buffer
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	//Load indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, &indexList[0], GL_STATIC_DRAW);

	//load vertices
	if (numVertices > 0) {
		position_loc = glGetAttribLocation(program, "vPosition");
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVertices, &vertexList[0]);
		glEnableVertexAttribArray(position_loc);
		glVertexAttribPointer(position_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	}

	//load normals right after vertices
	if (numNormals > 0) {
		normal_loc = glGetAttribLocation(program, "vNormal");
		glBufferSubData(GL_ARRAY_BUFFER, sizeVertices, sizeNormals, &normalList[0]);
		glEnableVertexAttribArray(normal_loc);
		glVertexAttribPointer(normal_loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeVertices));
	}

	//load UVs after vertices and normals
	if (numUVs > 0) {
		uv_loc = glGetAttribLocation(program, "tCorrdinate");
		glBufferSubData(GL_ARRAY_BUFFER, sizeVertices + sizeNormals, sizeUVs, &uvList[0]);
		glEnableVertexAttribArray(uv_loc);
		glVertexAttribPointer(uv_loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeVertices + sizeNormals));
	}

	//load texture if available
	if (textureAvailable) {
		cout << "Texture: " << textureFileName << endl;

		//Initialize texture
		texture.Init(textureFileName.c_str(), GL_TEXTURE0);
	}
	else {
		cout << "No texture" << endl;
	}

	// unbind the buffer and vao
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

float Mesh::MaxWidth()
{
	return BoxMax;
}

vec4 Mesh::BoxCenter()
{
	return centerBox;
}

bool Mesh::Draw(GLenum mode)
{
	//bind vao and vbo
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	//move material properties to shader
	GLuint ambient_loc, diffuse_loc,
		specular_loc, shin_loc, emission_loc;
	ambient_loc = glGetUniformLocation(program, "MAmbient");
	diffuse_loc = glGetUniformLocation(program, "MDiffuse");
	specular_loc = glGetUniformLocation(program, "MSpecular");
	shin_loc = glGetUniformLocation(program, "Shininess");
	emission_loc = glGetUniformLocation(program, "MEmission");

	glUniform3fv(ambient_loc, 1, ambient);
	glUniform3fv(diffuse_loc, 1, diffuse);
	glUniform3fv(specular_loc, 1, specular);
	glUniform3fv(emission_loc, 1, emission);
	glUniform1f(shin_loc, shininess);

	//set texturing in shader
	glUniform1i(glGetUniformLocation(program, "textureused"),
		textureAvailable && uvList.size() > 0);

	if (textureAvailable) {
		texture.Use(program, "tex");
	}

	glDrawElements(mode, indexList.size(), GL_UNSIGNED_INT, (void*)0);
	//glDrawArrays(mode, 0, vertexList.size());

	//unbind index buffer and vao
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return true;
}

vec3 Mesh::MinBoxPt()
{
	return boundBox[1];
}

vec3 Mesh::MaxBoxPt()
{
	return boundBox[7];
}

Mesh::~Mesh()
{
	vertexList.clear();
	normalList.clear();
	uvList.clear();
}
