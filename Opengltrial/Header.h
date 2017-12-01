
#ifndef kk
#define kk

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "glew.h"
#include "freeglut.h"
#include <iostream>
#include <fstream>
#include <string>
#include <Algorithm>
#include <filesystem>
#include <experimental/filesystem>
#include <ctime>

#include "Matrix4.h"
#include "model_obj.h"
#include "Vector3.h"
#include "Shaders.h"
#include "lodepng.h"

enum Shadertype {     // different types of shaders
	FragmentIllumination,
	Vertexillumination
};

enum Projections
{
	Orthogonal,
	Perspective
};

struct Texture {
	GLuint TextureObject = 0;				///< A texture object
	unsigned int TextureWidth = 0;			///< The width of the current texture
	unsigned int TextureHeight = 0;			///< The height of the current texture
	unsigned char *TextureData = nullptr;	///< the array where the texture image will be stored
	void Delete() {
		if (TextureObject != 0)
			glDeleteTextures(1, &TextureObject);
		if (TextureData != nullptr)
			free(TextureData);
	}
};

struct MyVertex
{
	Vector3f position;
	float texcoords[2];
	Vector3f normal;
};

struct MyModel {
	void Reset() {
		vertices.clear();
		indices.clear();
		if (texture0.TextureData != nullptr) {
			free(texture0.TextureData);
			texture0.TextureData = nullptr;
		}

		glDeleteBuffers(1, &IBO);
		glDeleteBuffers(1, &VBO);
	}

	std::vector<MyVertex> vertices;
	std::vector<int> indices;
	GLuint VBO = 0;	// A vertex buffer object
	GLuint IBO = 0;	// An index buffer object
	Texture texture0;

	int GetNumberOfTriangles() {
		return indices.size() / 3;
	}

	int Getnumberofvertices() {
		return vertices.size();
	}

	void GenerateNormals();
};

struct Light {
	Light() {
		Reset();
	}

	void Reset() {
		lightAColor = { 0.05f, 0.03f, 0.0f };
		lightDColor = { 1.0f, 1.0f, 1.0f };
		lightSColor = { 0.6f, 0.6f, 0.7f };
		lightAIntensity= 1.0f;
		lightDIntensity = 1.0f;
		lightSIntensity = 1.0f;
	}
	GLint lightAColorLoc = -1;
	GLint lightDColorLoc = -1;
	GLint lightSColorLoc = -1;
	GLint lightAIntensityLoc = -1;
	GLint lightDIntensityLoc = -1;
	GLint lightSIntensityLoc = -1;

	Vector3f lightAColor;
	Vector3f lightDColor;
	Vector3f lightSColor;
	float lightAIntensity = -1;
	float lightDIntensity = -1;
	float lightSIntensity = -1;
	
};

struct DirectionalLight : Light {
	DirectionalLight() {
		Reset();
	};

	void Reset() {
		Light::Reset();
		dLightDir = { 0.0f, -0.5f, -1.0f };
	}

	GLint dLightDirLoc=-1;
	Vector3f dLightDir;
};

struct HeadLight : Light {        //spot light mounted on head with restricted lightning angle and decreasing light intensity with distance
	HeadLight() {
		Reset();
	}
	void Reset() {
		Light::Reset();
		klinear = 0.0001f;
		ksquared = 0.001f;
		anglerestriction = 40;
		angledecreasecoef = 10;
	}

	float klinear;
	float ksquared;
	float angledecreasecoef;
	int anglerestriction;

	GLint anglerestictionloc = -1;
	GLint klinearloc=-1;
	GLint ksquaredloc=-1;
	GLint angledecreasecoefloc = -1;

};

struct Camera {
	Vector3f position;	// the position of the camera
	Vector3f target;	// the direction the camera is looking at
	Vector3f up;		// the up vector of the camera

	float fov;			// camera field of view
	float ar;			// camera aspect ratio

	float zNear, zFar;	// depth of the near and far plane

	float zoom;			// an additional scaling parameter

	//Compute camera transformation matrix
	Matrix4f computeCameraTransform();          
	Matrix4f computeTransformToworldcoordinates();      //compute matrix that transform object to world coordinates
	Matrix4f computeNormalTransform();					//copute matrix that transform object'snormal to normals in wolrds coordinates

	//Reset camera to default settings
	void Reset();

	Projections projection=Perspective;

	void Setpath(std::vector<Vector3f> points);  //set the control points
	void Setpath2(std::vector<Vector3f> points);  //set the control points


	Vector3f Evaluatepoint(float t);
	Vector3f Evaluatepoint2(float t);

	std::vector<Vector3f> ctrlpoints;
	std::vector<Vector3f> ctrlpoints2;
	std::vector<long long> binomialcoef;
	std::vector<long long> binomialcoef2;

};



struct Modelclass {
	ModelOBJ Model;		// A 3D model
	GLuint VBO = 0;	// A vertex buffer object
	GLuint IBO = 0;	// An index buffer object
	std::vector<Texture> maintexture;            //stored textures
};


struct SkyBox{
	void Init() {
		// cube vertices for vertex buffer object
		vertices = {
			-1.0,  1.0,  1.0,
			-1.0, -1.0,  1.0,
			1.0, -1.0,  1.0,
			1.0,  1.0,  1.0,
			-1.0,  1.0, -1.0,
			-1.0, -1.0, -1.0,
			1.0, -1.0, -1.0,
			1.0,  1.0, -1.0,
		};
		
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);

		// cube indices for index buffer object
		indices = {
			0, 3, 2, 1,
			3, 7, 6, 2,
			7, 4, 5, 6,
			4, 0, 1, 5,
			0, 4, 7, 3,
			1, 2, 6, 5,
		};
		//soren henning
		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*indices.size(), indices.data(), GL_STATIC_DRAW);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	std::vector<float> vertices;
	std::vector<int> indices;
	GLuint VBO;
	GLuint IBO;
	bool Load(std::string vertexshader, std::string fragmentshader);
	bool Loadtextures();

	GLuint ShaderProgram = 0;	// A shader program

	GLuint Matrixloc = -1;

	GLuint smaplerloc = -1;
	GLuint cubemaptexture = -1;
};

class Programm {
public:
	Programm() {
		Cam.Reset();
		lasttime = clock();
		texturevsmaterialindex = 1.0;
		Cam.Setpath({ { 0.0f, 0.0f, 5.0f },{ 10.0f, 0.0f, 5.0f },{ 3.0f, 0.5f, -8.0f },{ 3.0f, 100.0f, -8.0f },{ -20.0f, 100.0f, -500.0f }, { -20.0f, 100.0f, -8.0f },{ -20.0f, 0.5f, -8.0f },{ -10.0f, 0.0f, 5.0f },{ 0.0f, 0.0f, 5.0f } });  
		Cam.Setpath2({ { 0.0f, 0.0f, -1.0f }, { 0.0f, -0.1f, -1.0f },{ 0.0f,  -0.1f, 0.0f }, { -1.0f,  -0.1f, 0.0f },{ 0.0f,  -0.1f, 1.0f },{ 1.0f, -0.1f, 0.0f },{ 0.0f, 0.0f, -1.0f },{ 0.0f, 0.0f, -1.0f } });
		Cam.Evaluatepoint(0.5);
	}


	// ************************************************************************************************
	// *** OpenGL callbacks definition ************************************************************

	void display();
	void keyboard(unsigned char key, int x, int y);
	void idle();
	void special(int key, int x, int y);
	void mouse(int, int, int, int);
	void motion(int, int);
	

	// *****************Othrer method definition************************

	

	///<summary>
	/// Initialize buffer objects
	///</summary>
	bool InitMesh(std::string location);

	// load terrain
	bool InitTerrain(std::string name);


	///<summary>
	///load shaders
	///<summary>
	bool LoadShaders(std::string vertexshader, std::string fragmentshader);

	///<summary>
	///load texture;
	///<summary>
	static bool Loadtexture(std::string texturename, Texture & texture, bool alfa = false);
	
	bool Loadmodel(Modelclass & model, std::string filename, std::string pathname);    //loads model(filename contains whole name of file and path name only path to directory

	// 3D model
	Modelclass building;

	bool fogenabled = false;
	GLint fogenabledloc = -1;
	bool movecamera = false;
	bool movecamera2 = false;
	bool coloursbyheight = false;
	GLint colourbyheightloc;

	float cameracoef = 0;
	bool rotatebuilding = true;
	double buildingangle = 0.0;

	Modelclass dragon;

	MyModel terrain;

	SkyBox skybox;

	GLuint VBOT = 0;	// A vertex buffer object of cube
	GLuint IBOT = 0;	// An index buffer object  of cube
	clock_t lasttime;
	double difference;

	std::vector<Texture> cinematexture;            //stored textures
	Texture bumptexture;
	double texturevsmaterialindex;                //how much does texture influence colour;
	GLint texturevsmaterialindexloc=-1; 
						
	GLuint ShaderProgram=0;	// A shader program
	Shadertype shadertype = FragmentIllumination;   //typeofshader currently used

	//Lights
	DirectionalLight dLight;
	HeadLight pLight;
	bool directional=true;
	

    // Vertex transformation
	Vector3f Translation;	// Translation
	float Scaling;			///< Scaling

	// Mouse interaction
	int MouseX, MouseY;		//The last position of the mouse
	int MouseButton;		//The last mouse button pressed or released
	Camera Cam;

	//Shader variables
	GLint Bumploc = -1;     //location of switch between normal shanding and bump mapping
	GLint Toonloc = -1;     //location of switch between normal shanding and toon shading
	GLint TrLocation = -1;  //location of shader transformation variable
	GLint PointTrLocation = -1;  //location of matrix trandofrming to wolrd coordinates
	GLint NormalTrLocation = -1;  //location of matrix trandofrming normals to wolrd coordinates

	GLint DirectionalLoc = -1;
	GLint CameraPositionLoc = -1;
	GLint CameraDirLoc = -1;              //position of camera direction(target) variable in shader
	GLint MaterialAColorLoc = -1;
	GLint MaterialDColorLoc = -1;
	GLint MaterialSColorLoc = -1;
	GLint MaterialShineLoc = -1;
};
#endif // !kk