#pragma once
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>

class Shaderloader
{
public:
	bool Initshaders(const char* vertexShaderFilename,
		const char* fragmentShaderFilename);
	GLuint shader;                        //actual shader
/*	GLint TrLocation = -1;                //location of transformation variable in shader
	
	GLint PointTrLocation = -1;           //trandofrmation to world coordinates
	GLint NormalTrLocation = -1;          //transofrmation of normals to world coordinates
	GLint CameraPositionLoc = -1;
	GLint CameraDirLoc = -1;
	GLint DirectionalLoc = -1;                     //true=directional light  false= head-mounted light
	GLint BumpLoc = -1;                     //true=applybump  false= not apply
	GLint Toonloc = -1;
	GLint texturevsmaterialindex = -1;
	GLint colourbyheightloc = -1;
	GLint DLightDirLoc = -1;
	GLint DLightAColorLoc = -1;
	GLint DLightDColorLoc = -1;
	GLint DLightSColorLoc = -1;
	GLint DLightAIntensityLoc = -1;
	GLint DLightDIntensityLoc = -1;
	GLint DLightSIntensityLoc = -1;

	GLint klinearloc = -1;
	GLint ksquaredloc = -1;
	GLint anglerestrictionloc = -1;
	GLint angledecreaseloc = -1;
	GLint PLightAColorLoc = -1;
	GLint PLightDColorLoc = -1;
	GLint PLightSColorLoc = -1;
	GLint PLightAIntensityLoc = -1;
	GLint PLightDIntensityLoc = -1;
	GLint PLightSIntensityLoc = -1;



	GLint MaterialAColorLoc = -1;
	GLint MaterialDColorLoc = -1;
	GLint MaterialSColorLoc = -1;
	GLint MaterialShineLoc = -1;*/
private:
	std::string readTextFile(const std::string& pathAndFileName);
};

