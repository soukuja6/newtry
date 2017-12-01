
#include "Header.h"
using namespace std;
using  namespace std::experimental::filesystem;


bool Programm::InitTerrain(string name) {

	glEvalCoord1f((GLfloat)10 / 30.0);
	//open the file
	int colsNum, rowsNum, NO_DATA;
	double xLowLeft, yLowLeft, cellSize;

	int numberOfPoints;
	vector<float> correctindeces;

	ifstream fileIn;
	fileIn.open(name.data(), ios::binary);
	rowsNum = 3072;
	colsNum = 2754;
	if (!fileIn.good()) {		//check if the file has been opened
		cout << "Error opening the terrain file." << endl;
		return false;
	}

	//struct to read values from the file
	union { char cVals[4]; int iVal; } buffer4;
	union { char cVals[8]; double dVal; } buffer8;
	//union { char* cVals; float* fVals; } bufferX;

	
	//read the header
	fileIn.read(buffer4.cVals, 4);
	colsNum = buffer4.iVal;
	fileIn.read(buffer4.cVals, 4);
	rowsNum = buffer4.iVal;
	fileIn.read(buffer8.cVals, 8);
	xLowLeft = buffer8.dVal;
	fileIn.read(buffer8.cVals, 8);
	yLowLeft = buffer8.dVal;
	fileIn.read(buffer8.cVals, 8);
	cellSize = buffer8.dVal;
	fileIn.read(buffer4.cVals, 4);
	NO_DATA = buffer4.iVal;

	//read the height values
	numberOfPoints = rowsNum * colsNum;
	//bufferX.fVals = new float[numberOfPoints];

	vector<float> buff(numberOfPoints);
	fileIn.read((char *)buff.data(), 4 * numberOfPoints);

	//copy the height values in the global array
	

	xLowLeft = 0;// I dont need it
	yLowLeft = 0;

	int truindex = 0;
	correctindeces.resize(numberOfPoints);
	for (int i = 0; i < numberOfPoints; ++i) {
		float height = buff[i];

		if (height > NO_DATA) {
			MyVertex vertex;
			vertex.position = { (float)(floor(i / rowsNum)*cellSize + yLowLeft) , height,  (float)((i%rowsNum)*cellSize + xLowLeft) };
			vertex.texcoords[0] = ( i / rowsNum) / (colsNum - 1.0f) ;
			vertex.texcoords[1] = ( i % rowsNum) / (rowsNum - 1.0f) ;
			if (vertex.texcoords[0] > 1 || vertex.texcoords[0] < 0 || vertex.texcoords[1] > 1 || vertex.texcoords[1] < 0) {
				cout << "chyba";
			}
			terrain.vertices.push_back(vertex);
			correctindeces[i] = truindex++;
		}
		else {
			correctindeces[i] = -1;
		}
	}
	fileIn.close(); //close the file
	
	

	for (int r = 0; r < rowsNum-1; r++) {
		for (int c = 0; c < colsNum-1; c++) {
			int i0 = correctindeces[c*rowsNum + r];
			int i1 = correctindeces[c*rowsNum + r+1];
			int i2 = correctindeces[(c+1)*rowsNum + r];
			int i3 = correctindeces[(c+1)*rowsNum + r + 1];

			//if (i0 == -1 || i1 == -1 || i2 == -1 || i3 == -1) continue;
			if ((terrain.vertices[i0].position - terrain.vertices[i3].position).magnitude() < (terrain.vertices[i1].position - terrain.vertices[i2].position).magnitude()) {
				if (!(i0 == -1 || i1 == -1 || i3 == -1)) {
					terrain.indices.push_back(i0);
					terrain.indices.push_back(i1);
					terrain.indices.push_back(i3);
				}
				if (!(i0 == -1 || i2 == -1 || i3 == -1)) {
					terrain.indices.push_back(i0);
					terrain.indices.push_back(i3);
					terrain.indices.push_back(i2);
				}
			}
			else {
				if (!(i0 == -1 || i2 == -1 || i1 == -1)) {
					terrain.indices.push_back(i0);
					terrain.indices.push_back(i1);
					terrain.indices.push_back(i2);
				}
				if (!(i1 == -1 || i2 == -1 || i3 == -1)) {
					terrain.indices.push_back(i2);
					terrain.indices.push_back(i1);
					terrain.indices.push_back(i3);
				}
			}
		}
	}


								// static vs dynamic
	
	terrain.GenerateNormals();

	// Generate a VBO as in the previous tutorial
	glGenBuffers(1, &terrain.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, terrain.VBO);
	glBufferData(GL_ARRAY_BUFFER,
		terrain.vertices.size() * sizeof(MyVertex),
		terrain.vertices.data(),
		GL_STATIC_DRAW);


	// Create a buffer
	glGenBuffers(1, &terrain.IBO);
	// Set it as a buffer for indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain.IBO);

	// Set the data for the current IBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,				// the target
		terrain.GetNumberOfTriangles() * 3 * sizeof(int), // the size of the data
		terrain.indices.data(),										// pointer to the data
		GL_STATIC_DRAW);

	if (!Loadtexture("terrain\\bergen_terrain_texture.png", terrain.texture0, true)) {
		return false;
	}
	return true;
}

void MyModel::GenerateNormals() {
	vector<Vector3f> trinaglenormals(GetNumberOfTriangles());
	for (int i = 0; i < GetNumberOfTriangles(); i++) {
		auto & v0 = vertices[indices[3 * i]].position;
		auto & v1 = vertices[indices[3 * i+1]].position;
		auto & v2 = vertices[indices[3 * i+2]].position;

		trinaglenormals[i] = (v1 - v0).cross(v2 - v0);
	}

	vector<vector<int>> trinaglespervertex(Getnumberofvertices());

	for (size_t i = 0; i < indices.size(); i++) {
		trinaglespervertex[indices[i]].push_back(i/3);
	}

	for (size_t i = 0; i < Getnumberofvertices(); i++) {
		vertices[i].normal = { 0,0,0 };
		for (auto && tri : trinaglespervertex[i]) {
			vertices[i].normal += trinaglenormals[tri];
		}
		vertices[i].normal.normalize();
	}


}


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

bool Programm::Loadtexture(std::string texturename, Texture & texture, bool alfa) {
	auto & act = texture;

	if (act.TextureData != nullptr)
		free(act.TextureData);
	auto colortype = LCT_RGB;
	if (alfa) colortype = LCT_RGBA;
	unsigned int fail = lodepng_decode_file(&act.TextureData, &act.TextureWidth, &act.TextureHeight,
		texturename.c_str(),
		colortype, 8); // Remember to check the last 2 parameters
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
	auto colortypee = GL_RGB;
	if (alfa) colortypee = GL_RGBA;
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		colortypee,			// remember to check this
		act.TextureWidth,
		act.TextureHeight,
		0,
		colortypee,			// remember to check this
		GL_UNSIGNED_BYTE,
		act.TextureData
	);

	// Configure texture parameter
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

bool Programm::LoadShaders(std::string vertexshader, std::string fragmentshader)
{
	Shaderloader loader;

	if (!loader.Initshaders(vertexshader.data(), fragmentshader.data()))
		return false;

	ShaderProgram = loader.shader;


	
	// Get the location of the uniform variables
	TrLocation = glGetUniformLocation(ShaderProgram, "transformation");
	PointTrLocation = glGetUniformLocation(ShaderProgram, "vertextransformation");
	NormalTrLocation = glGetUniformLocation(ShaderProgram, "normaltransformation");
	CameraPositionLoc = glGetUniformLocation(ShaderProgram, "camera_position");
	CameraDirLoc = glGetUniformLocation(ShaderProgram, "cameradir");
	DirectionalLoc = glGetUniformLocation(ShaderProgram, "directional");
	Bumploc = glGetUniformLocation(ShaderProgram, "applybump");
	Toonloc = glGetUniformLocation(ShaderProgram, "applytoon");
	texturevsmaterialindexloc = glGetUniformLocation(ShaderProgram, "tex_vs_mat");
	colourbyheightloc = glGetUniformLocation(ShaderProgram, "colourbyheight");

	dLight.dLightDirLoc = glGetUniformLocation(ShaderProgram, "d_light_direction");
	dLight.lightAColorLoc = glGetUniformLocation(ShaderProgram, "d_light_a_color");
	dLight.lightDColorLoc = glGetUniformLocation(ShaderProgram, "d_light_d_color");
	dLight.lightSColorLoc = glGetUniformLocation(ShaderProgram, "d_light_s_color");
	dLight.lightAIntensityLoc = glGetUniformLocation(ShaderProgram, "d_light_a_intensity");
	dLight.lightDIntensityLoc = glGetUniformLocation(ShaderProgram, "d_light_d_intensity");
	dLight.lightSIntensityLoc = glGetUniformLocation(ShaderProgram, "d_light_s_intensity");

	pLight.klinearloc = glGetUniformLocation(ShaderProgram, "klinear");
	pLight.ksquaredloc = glGetUniformLocation(ShaderProgram, "ksquared");
	pLight.anglerestictionloc = glGetUniformLocation(ShaderProgram, "anglerest");
	pLight.angledecreasecoefloc = glGetUniformLocation(ShaderProgram, "angledecrease");
	pLight.lightAColorLoc = glGetUniformLocation(ShaderProgram, "p_light_a_color");
	pLight.lightDColorLoc = glGetUniformLocation(ShaderProgram, "p_light_d_color");
	pLight.lightSColorLoc = glGetUniformLocation(ShaderProgram, "p_light_s_color");
	pLight.lightAIntensityLoc = glGetUniformLocation(ShaderProgram, "p_light_a_intensity");
	pLight.lightDIntensityLoc = glGetUniformLocation(ShaderProgram, "p_light_d_intensity");
	pLight.lightSIntensityLoc = glGetUniformLocation(ShaderProgram, "p_light_s_intensity");


	MaterialAColorLoc = glGetUniformLocation(ShaderProgram, "material_a_color");
	MaterialDColorLoc = glGetUniformLocation(ShaderProgram, "material_d_color");
	MaterialSColorLoc = glGetUniformLocation(ShaderProgram, "material_s_color");
	MaterialShineLoc = glGetUniformLocation(ShaderProgram, "material_shininess");

	fogenabledloc = glGetUniformLocation(ShaderProgram, "fogenabled");

	assert(TrLocation != -1);  // check for errors (variable not found)

	return true;
}

Matrix4f Camera::computeTransformToworldcoordinates() {
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
		prj = Matrix4f::createOrthoPrj(-ar*x / 2.0f, ar*x / 2.0f, -x / 2.0f, x / 2.0f, zNear, zFar);
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
	return prj * camR;
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
	position.set(0.0f, 0.0f, 5.0f);
	target.set(0.f, 0.f, -1.f);
	up.set(0.f, 1.f, 0.f);
	fov = 50.f;
	ar = 1.f;  // will be correctly initialized in the "display()" method
	zNear = 0.1f;
	zFar = 1000.f;
	zoom = 1.f;
}

void Camera::Setpath(std::vector<Vector3f> points) {
	ctrlpoints = points;
	int n = points.size()-1;
	long long k, i;
	binomialcoef.clear();
	binomialcoef.resize(n+1);
	for (k = 0; k <= n; k++) {
		binomialcoef[k] = 1;
		for (i = n; i >= k + 1; i--)
			binomialcoef[k] *= i;
		for (i = n - k; i >= 2; i--)
			binomialcoef[k] /= i;
	}
}

void Camera::Setpath2(std::vector<Vector3f> points) {
	ctrlpoints2 = points;
	int n = points.size() - 1;
	long long k, i;
	binomialcoef2.clear();
	binomialcoef2.resize(n + 1);
	for (k = 0; k <= n; k++) {
		binomialcoef2[k] = 1;
		for (i = n; i >= k + 1; i--)
			binomialcoef2[k] *= i;
		for (i = n - k; i >= 2; i--)
			binomialcoef2[k] /= i;
	}
}

Vector3f Camera::Evaluatepoint(float t) {
	Vector3f point = {0,0,0};
	int n = ctrlpoints.size()-1;
	for (int k = 0; k<=n; k++) {
		float el = binomialcoef[k] * pow(t, k)*pow(1 - t, n - k);
		point += ctrlpoints[k] * el;
		
	}
	return point;
}

Vector3f Camera::Evaluatepoint2(float t) {
	Vector3f point = { 0,0,0 };
	int n = ctrlpoints2.size() - 1;
	for (int k = 0; k <= n; k++) {
		float el = binomialcoef2[k] * pow(t, k)*pow(1 - t, n - k);
		point += ctrlpoints2[k] * el;

	}
	return point;
}

bool SkyBox::Load(std::string vertexshader, std::string fragmentshader) {
	Shaderloader loader;

	if (!loader.Initshaders(vertexshader.data(), fragmentshader.data()))
		return false;

	ShaderProgram = loader.shader;

	Matrixloc = glGetUniformLocation(ShaderProgram, "matrix");
	smaplerloc = glGetUniformLocation(ShaderProgram, "cubemap");
	
	return true;
}

bool SkyBox::Loadtextures() {
	vector<Texture> textures(6);
	glGenTextures(1, &cubemaptexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaptexture);

	GLint types[] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };
	vector<string> names = { "xpos.png","xneg.png","ypos.png","yneg.png","zpos.png","zneg.png" };
	for (size_t i = 0; i < 6; i++) {
		if (!Programm::Loadtexture("Skybox\\" + names[i], textures[i]))
		 return false;


		glTexImage2D(
			types[i],
			0,
			GL_RGB,			// remember to check this
			textures[i].TextureWidth,
			textures[i].TextureHeight,
			0,
			GL_RGB,			// remember to check this
			GL_UNSIGNED_BYTE,
			textures[i].TextureData
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (size_t i = 0; i < 6; i++) {
		textures[i].Delete();
	}
	return true;
}
