#include "Shaders.h"
using namespace std;



// Initialize shaders. Return 0 if initialization fail
bool Shaderloader::Initshaders(const char * vertexShaderFilename, const char * fragmentShaderFilename)
{
	// Create the shader program and check for errors
	
	GLuint ShaderProgram = glCreateProgram();
	if (ShaderProgram == 0) {
		cerr << "Error: cannot create shader program." << endl;
		return false;
	}

	// Create the shader objects and check for errors
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (vertShader == 0 || fragShader == 0) {
		cerr << "Error: cannot create shader objects." << endl;
		return false;
	}

	// Read and set the source code for the vertex shader
	string text = readTextFile(vertexShaderFilename);
	const char* code = text.c_str();
	int length = static_cast<int>(text.length());
	if (length == 0)
		return false;
	glShaderSource(vertShader, 1, &code, &length);

	// Read and set the source code for the fragment shader
	string text2 = readTextFile(fragmentShaderFilename);
	const char *code2 = text2.c_str();
	length = static_cast<int>(text2.length());
	if (length == 0)
		return false;
	glShaderSource(fragShader, 1, &code2, &length);

	// Compile the shaders
	glCompileShader(vertShader);
	glCompileShader(fragShader);

	// Check for compilation error
	GLint success;
	GLchar errorLog[1024];
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertShader, 1024, nullptr, errorLog);
		cerr << "Error: cannot compile vertex shader.\nError log:\n" << errorLog << endl;
		return false;
	}
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragShader, 1024, nullptr, errorLog);
		cerr << "Error: cannot compile fragment shader.\nError log:\n" << errorLog << endl;
		return false;
	}

	// Attach the shader to the program and link it
	glAttachShader(ShaderProgram, vertShader);
	glAttachShader(ShaderProgram, fragShader);
	glLinkProgram(ShaderProgram);

	// Check for linking error
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ShaderProgram, 1024, nullptr, errorLog);
		cerr << "Error: cannot link shader program.\nError log:\n" << errorLog << endl;
		return false;
	}

	// Make sure that the shader program can run
	glValidateProgram(ShaderProgram);

	// Check for validation error
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ShaderProgram, 1024, nullptr, errorLog);
		cerr << "Error: cannot validate shader program.\nError log:\n" << errorLog << endl;
		return false;
	}

	/*
	// Get the location of the uniform variables
	TrLocation = glGetUniformLocation(ShaderProgram, "transformation");
	PointTrLocation = glGetUniformLocation(ShaderProgram, "vertextransformation");
	NormalTrLocation = glGetUniformLocation(ShaderProgram, "normaltransformation");
	CameraPositionLoc = glGetUniformLocation(ShaderProgram, "camera_position");
	CameraDirLoc = glGetUniformLocation(ShaderProgram, "cameradir");
	DirectionalLoc = glGetUniformLocation(ShaderProgram, "directional");
	BumpLoc = glGetUniformLocation(ShaderProgram, "applybump");
	Toonloc = glGetUniformLocation(ShaderProgram, "applytoon");
	texturevsmaterialindex = glGetUniformLocation(ShaderProgram, "tex_vs_mat");
	colourbyheightloc = glGetUniformLocation(ShaderProgram, "colourbyheight");

	DLightDirLoc = glGetUniformLocation(ShaderProgram, "d_light_direction");
	DLightAColorLoc = glGetUniformLocation(ShaderProgram, "d_light_a_color");
	DLightDColorLoc = glGetUniformLocation(ShaderProgram, "d_light_d_color");
	DLightSColorLoc = glGetUniformLocation(ShaderProgram, "d_light_s_color");
	DLightAIntensityLoc = glGetUniformLocation(ShaderProgram, "d_light_a_intensity");
	DLightDIntensityLoc = glGetUniformLocation(ShaderProgram, "d_light_d_intensity");
	DLightSIntensityLoc = glGetUniformLocation(ShaderProgram, "d_light_s_intensity");

	klinearloc = glGetUniformLocation(ShaderProgram, "klinear");
	ksquaredloc = glGetUniformLocation(ShaderProgram, "ksquared");
	anglerestrictionloc = glGetUniformLocation(ShaderProgram, "anglerest");
	angledecreaseloc = glGetUniformLocation(ShaderProgram, "angledecrease");
	PLightAColorLoc = glGetUniformLocation(ShaderProgram, "p_light_a_color");
	PLightDColorLoc = glGetUniformLocation(ShaderProgram, "p_light_d_color");
	PLightSColorLoc = glGetUniformLocation(ShaderProgram, "p_light_s_color");
	PLightAIntensityLoc = glGetUniformLocation(ShaderProgram, "p_light_a_intensity");
	PLightDIntensityLoc = glGetUniformLocation(ShaderProgram, "p_light_d_intensity");
	PLightSIntensityLoc = glGetUniformLocation(ShaderProgram, "p_light_s_intensity");


	MaterialAColorLoc = glGetUniformLocation(ShaderProgram, "material_a_color");
	MaterialDColorLoc = glGetUniformLocation(ShaderProgram, "material_d_color");
	MaterialSColorLoc = glGetUniformLocation(ShaderProgram, "material_s_color");
	MaterialShineLoc = glGetUniformLocation(ShaderProgram, "material_shininess");
	
	//assert(TrLocation != -1);  // check for errors (variable not found)*/


	// Shaders can be deleted now
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	shader = ShaderProgram;
	return true;
}


// Read the specified file and return its content
string  Shaderloader::readTextFile(const string& pathAndFileName) {
	// Try to open the file
	ifstream fileIn(pathAndFileName);
	if (!fileIn.is_open()) {
		cerr << "Error: cannot open file " << pathAndFileName.c_str();
		return "";
	}

	// Read the file
	string text = "";
	string line;
	while (!fileIn.eof()) {
		getline(fileIn, line);
		text += line + "\n";
		bool bad = fileIn.bad();
		bool fail = fileIn.fail();
		if (fileIn.bad() || (fileIn.fail() && !fileIn.eof())) {
			cerr << "Warning: problems reading file " << pathAndFileName.c_str()
				<< "\nBad flag: " << bad << "\tFail flag: " << fail
				<< "\nText read: \n" << text.c_str();
			fileIn.close();
			return text;
		}
	}
	// finalize
	fileIn.close();

	return text;
} /* readTextFile() */