#include "Object.h"
#include "soil.h"
#include <fstream>

Object::Object()
{

}

Object::Object(const char* filename)
{
	LoadFromFile(filename); 
}

float Object::MaxWidth()
{
	return BoxMax;
}

vec4 Object::BoxCenter()
{
	return centerBox;
}

//Load object from specific file
bool Object::LoadFromFile(const char* filename)
{
	this->filename = filename;

	//Use Assimp's importer to load object
	Assimp::Importer importer;
	const aiScene* scene = NULL;

	string fileloc = ".\\models\\" + string(filename);
	ifstream fin(fileloc);
	if (fin.fail()) {
		cout << "Couldn't open file:" << fileloc;
		return false;
	}
	scene = importer.ReadFile(fileloc.c_str(), aiProcessPreset_TargetRealtime_Quality);

	//if error occurs, print error
	if (!scene) {
		cout << importer.GetErrorString();
		return false;
	}

	cout << "Import of scene succeeded: " << fileloc << endl;
	
	if (scene->HasTextures()) {
		cout << "Support for meshes with embedded textures is not implemented" << endl;
		exit(1);
	}

	vec3 minBox(FLT_MAX), maxBox(-FLT_MAX), maxPt, minPt;

	//load all meshes for object
	for (int meshIdx = 0; meshIdx < scene->mNumMeshes; meshIdx++) {
		meshes.push_back(Mesh(scene, meshIdx));

		//get mesh's min and max box points
		maxPt = meshes.back().MaxBoxPt();
		minPt = meshes.back().MinBoxPt();

		//get 8 points for bounding box
		maxBox.x = max(maxPt.x, maxBox.x);
		maxBox.y = max(maxPt.y, maxBox.y);
		maxBox.z = max(maxPt.z, maxBox.z);
		minBox.x = min(minPt.x, minBox.x);
		minBox.y = min(minPt.y, minBox.y);
		minBox.z = min(minPt.z, minBox.z);
	}

	std::cout << "Meshes: " << meshes.size() << endl;

	//get box max
	BoxMax = max(max(maxBox.x - minBox.x,
		maxBox.y - minBox.y), maxBox.z - minBox.z);

	//Store 8 Axis Align bounding box points
	boundBox.push_back(vec3(minBox.x, maxBox.y, minBox.z)); //back left top
	boundBox.push_back(vec3(minBox.x, minBox.y, minBox.z)); //back left down
	boundBox.push_back(vec3(maxBox.x, minBox.y, minBox.z)); //back right down
	boundBox.push_back(vec3(maxBox.x, maxBox.y, minBox.z)); //back right top
	boundBox.push_back(vec3(minBox.x, maxBox.y, maxBox.z)); //front left top
	boundBox.push_back(vec3(minBox.x, minBox.y, maxBox.z)); //front left down
	boundBox.push_back(vec3(maxBox.x, minBox.y, maxBox.z)); //front right down
	boundBox.push_back(vec3(maxBox.x, maxBox.y, maxBox.z)); //front right top

	//Get center of bounding box
	centerBox = vec4((maxBox.x + minBox.x) / 2,
		(minBox.y + maxBox.y) / 2,
		(maxBox.z + minBox.z) / 2, 1.0);

	//Free Scene data
	importer.FreeScene();
}

Object::~Object(void)
{
	meshes.clear();
}

bool Object::LoadGPU(GLuint& prog)
{
	program = prog;
	bool loaded = true;

	cout << "\nLOADING OBJECT: " << filename << endl;
	//load all meshes
	for (Mesh& mesh : meshes)
	{
		loaded = loaded && mesh.LoadGPU(program);
	}
	return loaded;
}

bool Object::Draw(GLenum mode)
{
	//draw all meshes
	bool drawn = false;

	for (Mesh& mesh : meshes) {
		drawn = mesh.Draw(mode) || drawn;
	}
	return drawn;
}

vec3 Object::MinBoxPt()
{
	return boundBox[1];
}

vec3 Object::MaxBoxPt()
{
	return boundBox[7];
}
