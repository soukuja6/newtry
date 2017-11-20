
#include "Header.h"
using namespace std;
using  namespace std::experimental::filesystem;

// Initialize buffer objects
bool Programm::InitMesh(string location) {
	//--------------------------------cube----------------------------------
	// Create an array of vectors representing the vertices (of a cube)
	MyVertex vertices[8];
	vertices[0].position = { -0.6f, -0.25f, -0.0f };
	vertices[1].position = { 0.6f, -0.25f, -0.0f };
	vertices[2].position = { -0.6f, 0.25f, -0.0f };
	vertices[3].position = { 0.6f, 0.25f, -0.0f };

	vertices[0].normal = { 0.0f, 0.0f, 0.5f };
	vertices[1].normal = { 0.0f, 0.0f, 0.5f };
	vertices[2].normal = { 0.0f, 0.0f, 0.5f };
	vertices[3].normal = {0.0f, 0.0f, 0.5f };



	vertices[0].texcoords[0] = 0;
	vertices[0].texcoords[1] = 1;
	vertices[1].texcoords[0] = 1;
	vertices[1].texcoords[1] = 1; 
	vertices[2].texcoords[0] = 0;
	vertices[2].texcoords[1] = 0;
	vertices[3].texcoords[0] = 1;
	vertices[3].texcoords[1] = 0;
	

	



	// Generate a VBO as in the previous tutorial
	glGenBuffers(1, &VBOT);
	glBindBuffer(GL_ARRAY_BUFFER, VBOT);
	glBufferData(GL_ARRAY_BUFFER,
		4 * sizeof(MyVertex),
		vertices,
		GL_STATIC_DRAW);

	// Create an array of indices representing the triangles (faces of the cube)
	// Refer to the cube image in the slides (TODO)
	unsigned int indices[3 * 2] = {
		0, 1, 2, //back face
		2, 1, 3,
		};

	// Create a buffer
	glGenBuffers(1, &IBOT);

	// Set it as a buffer for indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOT);

	// Set the data for the current IBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,				// the target
		3 * 2 * sizeof(unsigned int), // the size of the data
		indices,										// pointer to the data
		GL_STATIC_DRAW);								// static vs dynamic


	auto path1 = current_path().append("cinema_textures");
	for (auto & p : directory_iterator(path1)) {
		auto name = p.path().string();
		cinematexture.emplace_back();
		
		if (!Loadtexture(name, cinematexture.back()))
			return false;

	}

	if (!Loadmodel(building, "building\\building.obj", "building\\"))
		return false;

	if (!Loadmodel(dragon, "dragon\\dragon.obj", "dragon\\"))
		return false;

	if (!Loadtexture("building\\bumpnormal.png", bumptexture))
		return false;
			
	return true;
} /* initBuffers() */

bool Programm::Loadmodel(Modelclass & model, string filename, string pathname) {
	//--------------load model-------------------------

	// Load the OBJ model
	if (!model.Model.import(filename.data())) {
		cerr << "Error: cannot load model." << endl;
		return false;
	}
	model.Model.normalize();
	model.Model.generateNormals();        // because the normalize function does not handle normals, and it probably should. Then i call it just for case in here.(I also had to do it public in the library.
	model.Model.generateTangents();
	// Notice that normals may not be stored in the model
	// This issue will be dealt with in the next lecture


	// VBO
	glGenBuffers(1, &model.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, model.VBO);
	glBufferData(GL_ARRAY_BUFFER,
		model.Model.getNumberOfVertices() * sizeof(ModelOBJ::Vertex),
		model.Model.getVertexBuffer(),
		GL_STATIC_DRAW);

	// IBO
	glGenBuffers(1, &model.IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		3 * model.Model.getNumberOfTriangles() * sizeof(int),
		model.Model.getIndexBuffer(),
		GL_STATIC_DRAW);


	// Check the materials for the texture
	int i = model.Model.getNumberOfMeshes();
	model.maintexture.clear();
	model.maintexture.resize(model.Model.getNumberOfMaterials());
	for (int i = 0; i < model.Model.getNumberOfMaterials(); ++i) {
		// if the current material has a texture
		if (model.Model.getMaterial(i).colorMapFilename != "") {

			if (!Loadtexture(pathname + model.Model.getMaterial(i).colorMapFilename, model.maintexture[i]))
				return false;
		}
	}
	return true;
}

bool Programm::Loadtexture(std::string texturename, Texture & texture) {
	auto & act = texture;

	if (act.TextureData != nullptr)
		free(act.TextureData);
	unsigned int fail = lodepng_decode_file(&act.TextureData, &act.TextureWidth, &act.TextureHeight,
		texturename.c_str(),
		LCT_RGB, 8); // Remember to check the last 2 parameters
	if (fail != 0) {
		cerr << "Error: cannot load texture file "
			<< texturename << endl;
		return false;
	}

	// Create the texture object
	if (act.TextureObject != 0)
		glDeleteTextures(1, &act.TextureObject);
	glGenTextures(1, &act.TextureObject);

	// Bind it as a 2D texture (note that other types of textures are supported as well)
	glBindTexture(GL_TEXTURE_2D, act.TextureObject);

	// Set the texture data
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,			// remember to check this
		act.TextureWidth,
		act.TextureHeight,
		0,
		GL_RGB,			// remember to check this
		GL_UNSIGNED_BYTE,
		act.TextureData
	);

	// Configure texture parameter
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

bool Programm::LoadShaders(std::string vertexshader, std::string fragmentshader)
{
	Shaderloader loader;

	if (!loader.Initshaders(vertexshader.data(), fragmentshader.data()))
		return false;

	ShaderProgram = loader.shader;
	TrLocation = loader.TrLocation;
	PointTrLocation = loader.PointTrLocation;
	NormalTrLocation = loader.NormalTrLocation;

	CameraPositionLoc = loader.CameraPositionLoc;
	CameraDirLoc = loader.CameraDirLoc;
	DirectionalLoc = loader.DirectionalLoc;
	Bumploc = loader.BumpLoc;
	Toonloc = loader.Toonloc;
	texturevsmaterialindexloc = loader.texturevsmaterialindex;

	dLight.dLightDirLoc = loader.DLightDirLoc;
	dLight.lightAColorLoc = loader.DLightAColorLoc;
	dLight.lightDColorLoc = loader.DLightDColorLoc;
	dLight.lightSColorLoc = loader.DLightSColorLoc;
	dLight.lightAIntensityLoc = loader.DLightAIntensityLoc;
	dLight.lightDIntensityLoc = loader.DLightDIntensityLoc;
	dLight.lightSIntensityLoc = loader.DLightSIntensityLoc;

	pLight.anglerestictionloc = loader.anglerestrictionloc;
	pLight.angledecreasecoefloc = loader.angledecreaseloc;
	pLight.klinearloc = loader.klinearloc;
	pLight.ksquaredloc = loader.ksquaredloc;
	pLight.lightAColorLoc = loader.PLightAColorLoc;
	pLight.lightDColorLoc = loader.PLightDColorLoc;
	pLight.lightSColorLoc = loader.PLightSColorLoc;
	pLight.lightAIntensityLoc = loader.PLightAIntensityLoc;
	pLight.lightDIntensityLoc = loader.PLightDIntensityLoc;
	pLight.lightSIntensityLoc = loader.PLightSIntensityLoc;


	MaterialAColorLoc = loader.MaterialAColorLoc;
	MaterialDColorLoc = loader.MaterialDColorLoc;
	MaterialSColorLoc = loader.MaterialSColorLoc;
	MaterialShineLoc = loader.MaterialShineLoc;
	return true;
}

Matrix4f Camera::computeTransformToworldcoordinates() {
	Vector3f t = target.getNormalized();
	Vector3f u = up.getNormalized();
	Vector3f r = t.cross(u);
	Matrix4f camR(r.x(), r.y(), r.z(), 0.f,			//axis x
		u.x(), u.y(), u.z(), 0.f,					// axis y 
		-t.x(), -t.y(), -t.z(), 0.f,               // axis z: axis z heads to oposite direction than the camera is looking at
		0.f, 0.f, 0.f, 1.f);

	// camera translation
	Matrix4f camT = Matrix4f::createTranslation(-position);     // minus because we are translation object and not camera
	return camR*camT;
};

Matrix4f Camera::computeNormalTransform() {
	Vector3f t = target.getNormalized();
	Vector3f u = up.getNormalized();
	Vector3f r = t.cross(u);
	Matrix4f camR(r.x(), r.y(), r.z(), 0.f,			//axis x
		u.x(), u.y(), u.z(), 0.f,					// axis y 
		-t.x(), -t.y(), -t.z(), 0.f,               // axis z: axis z heads to oposite direction than the camera is looking at
		0.f, 0.f, 0.f, 1.f);

	// camera translation
	Matrix4f camT = Matrix4f::createTranslation(-position);     // minus because we are translation object and not camera
	return (camR*camT).getInverse().getTransposed();
};

// Return the transformation matrix corresponding to the specified camera
Matrix4f Camera::computeCameraTransform() {
	// camera rotation

	Vector3f t = target.getNormalized();
	Vector3f u = up.getNormalized();
	Vector3f r = t.cross(u);
	Matrix4f camR(r.x(), r.y(), r.z(), 0.f,			//axis x
		u.x(), u.y(), u.z(), 0.f,					// axis y 
		-t.x(), -t.y(), -t.z(), 0.f,               // axis z: axis z heads to oposite direction than the camera is looking at
		0.f, 0.f, 0.f, 1.f);

	// camera translation
	Matrix4f camT = Matrix4f::createTranslation(-position);     // minus because we are translation object and not camera

																// perspective projection
	Matrix4f prj;
	if (projection == Projections::Perspective)
		prj = Matrix4f::createPerspectivePrj(fov, ar, zNear, zFar);
	else {
		int x = 2;
		prj = Matrix4f::createOrthoPrj(-ar*x/2.0f, ar*x / 2.0f, -x / 2.0f, x / 2.0f, zNear, zFar);
		/*prj = Matrix4f(1.f, 0.f, 0.f, 0.f,                          //orthogonal projection, just get rid of z axis
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 0.f, 0.f,
			0.f, 0.f, 0.f, 1.f);*/
	}
	// scaling due to zooming
	Matrix4f camZoom = Matrix4f::createScaling(zoom, zoom, 1.f);

	// Final transformation. Notice the multiplication order
	// First vertices are moved in camera space
	// Then the perspective projection puts them in clip space
	// And a final zooming factor is applied in clip space
	return camZoom * prj * camR  * camT;

} /* computeCameraTransform() */



void Camera::Reset()
{
	position.set(0.0f, 0.5f, 2.0f);
	target.set(0.f, 0.f, -1.f);
	up.set(0.f, 1.f, 0.f);
	fov = 30.f;
	ar = 1.f;  // will be correctly initialized in the "display()" method
	zNear = 0.1f;
	zFar = 100.f;
	zoom = 1.f;
}
