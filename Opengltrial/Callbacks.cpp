
#include "Header.h"
using namespace std;


// ************************************************************************************************
// *** OpenGL callbacks implementation ************************************************************

// Called whenever the scene has to be drawn
void Programm::display() {
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0, 0, width, height);

	// Enable depth test
	glDisable(GL_DEPTH_TEST);

	
	



	// Set the camera transformation
	Cam.ar = (1.0f * width) / height;
	Matrix4f transformation = Cam.computeCameraTransform();

	Matrix4f vertextoworldtr;
	Matrix4f normaltr;

	{  //-----------------------------------------------------------------skybox-----------------------------------------------------------
		glUseProgram(skybox.ShaderProgram); 

		glUniformMatrix4fv(skybox.Matrixloc, 1, GL_FALSE, (Cam.computeTransformToworldcoordinates()).get());

		glUniform1i(skybox.smaplerloc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.cubemaptexture);

		glBindBuffer(GL_ARRAY_BUFFER, skybox.VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox.IBO);

		glEnableVertexAttribArray(0);      

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			0,
			reinterpret_cast<const GLvoid*>(0));

		glDrawElements(GL_QUADS, skybox.indices.size(), GL_UNSIGNED_INT, 0);

	}

	glEnable(GL_DEPTH_TEST);
	// Enable the shader program
	assert(ShaderProgram != 0);
	glUseProgram(ShaderProgram);

	

	//SET SHADER VARIABLES
	glUniformMatrix4fv(TrLocation, 1, GL_FALSE, transformation.get());

	glUniform3fv(CameraPositionLoc, 1, Cam.position.get());
	glUniform3fv(CameraDirLoc, 1, Cam.target.get());
	glUniform1ui(colourbyheightloc, false);


	if (directional) {  //if the directional light is enabled
		glUniform1ui(DirectionalLoc, true);
		glUniform3fv(dLight.dLightDirLoc,1, dLight.dLightDir.get());
		//glUniform3f(DLight.DLightDirLoc, 0.5f, 0.5f, 0.5f);
		glUniform3fv(dLight.lightAColorLoc, 1,dLight.lightAColor.get());
		glUniform3fv(dLight.lightDColorLoc, 1,dLight.lightDColor.get());
		glUniform3fv(dLight.lightSColorLoc, 1,dLight.lightSColor.get());
		glUniform1f(dLight.lightAIntensityLoc, dLight.lightAIntensity);
		glUniform1f(dLight.lightDIntensityLoc, dLight.lightDIntensity);
		glUniform1f(dLight.lightSIntensityLoc, dLight.lightSIntensity);
	}
	else {     //otherwise it is head mounted light
		glUniform1ui(DirectionalLoc, false);
		glUniform1f(pLight.klinearloc, pLight.klinear);
		glUniform1f(pLight.ksquaredloc, pLight.ksquared);
		glUniform1f(pLight.anglerestictionloc, pLight.anglerestriction);
		glUniform1f(pLight.angledecreasecoefloc, pLight.angledecreasecoef);
		glUniform3f(pLight.lightAColorLoc, pLight.lightAColor.x(), pLight.lightAColor.y(), pLight.lightAColor.z());
		glUniform3f(pLight.lightDColorLoc, pLight.lightDColor.x(), pLight.lightDColor.y(), pLight.lightDColor.z());
		glUniform3f(pLight.lightSColorLoc, pLight.lightSColor.x(), pLight.lightSColor.y(), pLight.lightSColor.z());
		glUniform1f(pLight.lightAIntensityLoc, pLight.lightAIntensity);
		glUniform1f(pLight.lightDIntensityLoc, pLight.lightDIntensity);
		glUniform1f(pLight.lightSIntensityLoc, pLight.lightSIntensity);
	}


	// Enable the vertex attributes and set their format
	GLuint SamplerLoc = glGetUniformLocation(ShaderProgram, "sampler");

	// Set the uniform variable for the texture unit (texture unit 0)
	glUniform1i(SamplerLoc, 0);



	// Enable the vertex attributes and set their format
	GLuint bumpSamplerLoc = glGetUniformLocation(ShaderProgram, "bumpsampler");

	// Set the uniform variable for the texture unit (texture unit 1)
	glUniform1i(bumpSamplerLoc, 1);

	// Enable texture unit 1 and bind the texture to it
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bumptexture.TextureObject);


	{    //-----------------------------------------------------------------draw building------------------------------------------
		
		vertextoworldtr = Matrix4f::createRotation(buildingangle * 360, Vector3f(0.0, 1.0, 0.0));
		normaltr = vertextoworldtr.getTransposed().getInverse();
		
		glUniformMatrix4fv(PointTrLocation, 1, GL_FALSE, vertextoworldtr.get());
		glUniformMatrix4fv(NormalTrLocation, 1, GL_FALSE, normaltr.get());


		glUniform1f(texturevsmaterialindexloc, texturevsmaterialindex);   // how much does the texture influence the colour
		// Bind the buffers
		glBindBuffer(GL_ARRAY_BUFFER, building.VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, building.IBO);

		glEnableVertexAttribArray(0);        //LOCATION is first
		glEnableVertexAttribArray(1);        //texturecoor
		glEnableVertexAttribArray(2);        //normal is third
		glEnableVertexAttribArray(3);        //tangent is fourth
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(ModelOBJ::Vertex),
			reinterpret_cast<const GLvoid*>(0));

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
			sizeof(ModelOBJ::Vertex),
			reinterpret_cast<const GLvoid*>(sizeof(float[3]) + sizeof(float[2])));

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
			sizeof(ModelOBJ::Vertex),
			reinterpret_cast<const GLvoid*>(sizeof(float[3]) + sizeof(float[2]) + sizeof(float[3])));

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
			sizeof(ModelOBJ::Vertex),
			reinterpret_cast<const GLvoid*>(sizeof(Vector3f)));

		// Enable texture unit 0 and bind the texture to it
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, maintexture[3].TextureObject);

		auto && Model = building.Model;

		for (size_t i = 0; i < Model.getNumberOfMeshes(); i++)            // draw every mesh separately
		{
			auto && actmesh = Model.getMesh(i);
			int j = 0;
			for (j = 0; j < Model.getNumberOfMaterials(); j++)
			{
				if (&Model.getMaterial(j) == actmesh.pMaterial) {
					break;
				}
			}

			glUniform1ui(Bumploc, i==0);											// only first mesh will have applied bump on it
			glUniform1ui(Toonloc, false);              // no toon shading 
			glUniform3fv(MaterialAColorLoc, 1, Model.getMaterial(j).ambient);
			glUniform3fv(MaterialDColorLoc, 1, Model.getMaterial(j).diffuse);
			glUniform3fv(MaterialSColorLoc, 1, Model.getMaterial(j).specular);
			glUniform1f(MaterialShineLoc, 20);										//shininess from blender is set to almost zero, which more or less destroy the picture so we set it manually

			glBindTexture(GL_TEXTURE_2D, building.maintexture[j].TextureObject);

			glDrawElements(
				GL_TRIANGLES,
				actmesh.triangleCount * 3,
				GL_UNSIGNED_INT,
				(void*)(actmesh.startIndex * sizeof(GLuint)));

		}
	}

	{
		//------------------------------------------------- draw gandalf gif--------------------------------------------------------
		glBindBuffer(GL_ARRAY_BUFFER, VBOT);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOT);

		glUniform1ui(Bumploc, false);             // no bump mapping 
		glUniform1ui(Toonloc, false);              // no toon shading 
		// Enable texture unit 0 and bind the texture to it
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, terrain.texture0.TextureObject);
		glBindTexture(GL_TEXTURE_2D, cinematexture[(int)(difference * cinematexture.size())].TextureObject);   //bind texture

		glUniform1f(texturevsmaterialindexloc, texturevsmaterialindex);   // how much does the texture influence the colour

		//set some rubish material
		glUniform3f(MaterialAColorLoc, 0, 0.5, 0.5);
		glUniform3f(MaterialDColorLoc, 0.5, 0.5, 0.5);
		glUniform3f(MaterialSColorLoc, 1.0, 1.0, 1.0);
		glUniform1f(MaterialShineLoc, 5);


		vertextoworldtr = Matrix4f::createScaling(10, 10,1)*Matrix4f::createTranslation(Vector3f{ 0.0, 0.2, -20.0 });
		normaltr.identity();
		glUniformMatrix4fv(PointTrLocation, 1, GL_FALSE, vertextoworldtr.get());
		glUniformMatrix4fv(NormalTrLocation, 1, GL_FALSE, normaltr.get());

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(MyVertex),
			reinterpret_cast<const GLvoid*>(0));

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
			sizeof(MyVertex),
			reinterpret_cast<const GLvoid*>(sizeof(float[3]) + sizeof(float[2])));

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
			sizeof(MyVertex),
			reinterpret_cast<const GLvoid*>(sizeof(Vector3f)));
		// Bind the buffers

		glDrawElements(
			GL_TRIANGLES,
			2 * 3,
			GL_UNSIGNED_INT,
			0);
	}


	{//---------------------------------------------------draw dragon---------------------------------------------------------------

		vertextoworldtr = Matrix4f::createScaling(3, 3, 3) * Matrix4f::createTranslation(Vector3f{1.0,0.5,-5.0});
		normaltr.identity();
		glUniform1ui(Bumploc, false);              // no bump mapping
		glUniform1ui(Toonloc, true);              // apply toon shading 

		glUniform1f(texturevsmaterialindexloc, 0.0);   // we don't have texture on drgon, so texture does not influence the look at all


		glUniformMatrix4fv(PointTrLocation, 1, GL_FALSE, vertextoworldtr.get());
		glUniformMatrix4fv(NormalTrLocation, 1, GL_FALSE, normaltr.get());

		glUniform3f(MaterialAColorLoc, 0, 0.5, 0.5);
		glUniform3f(MaterialDColorLoc, 0, 0.5, 0.5);
		glUniform3f(MaterialSColorLoc, 1.0, 1.0, 1.0);
		glUniform1f(MaterialShineLoc, 20);


		// Bind the buffers
		glBindBuffer(GL_ARRAY_BUFFER, dragon.VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dragon.IBO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(ModelOBJ::Vertex),
			reinterpret_cast<const GLvoid*>(0));

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
			sizeof(ModelOBJ::Vertex),
			reinterpret_cast<const GLvoid*>(sizeof(float[3]) + sizeof(float[2])));

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
			sizeof(ModelOBJ::Vertex),
			reinterpret_cast<const GLvoid*>(sizeof(Vector3f)));

		glDrawElements(
			GL_TRIANGLES,
			dragon.Model.getNumberOfTriangles() * 3,
			GL_UNSIGNED_INT,
			0);	
	
	}


	{//---------------------------------------------------draw terrain---------------------------------------------------------------

		vertextoworldtr = Matrix4f::createScaling(0.05, 0.05, 0.05)*Matrix4f::createTranslation(Vector3f{ -6000.0f, -39.0f, -7000.0f });
		normaltr.identity();						//normals dont have to change with translation and scaling;
		glUniform1ui(Bumploc, false);              // no bump mapping
		glUniform1ui(Toonloc, false);              // apply toon shading 

		glUniform1f(texturevsmaterialindexloc, texturevsmaterialindex);
		glUniform1ui(colourbyheightloc, coloursbyheight);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, terrain.texture0.TextureObject);

		glUniformMatrix4fv(PointTrLocation, 1, GL_FALSE, vertextoworldtr.get());
		glUniformMatrix4fv(NormalTrLocation, 1, GL_FALSE, normaltr.get());

		glUniform3f(MaterialAColorLoc, 0, 0.5, 0.5);
		glUniform3f(MaterialDColorLoc, 0.5, 0.5, 0.5);
		glUniform3f(MaterialSColorLoc, 0.1, 0.1, 0.1);
		glUniform1f(MaterialShineLoc, 20);


		// Bind the buffers
		glBindBuffer(GL_ARRAY_BUFFER, terrain.VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain.IBO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(MyVertex),
			reinterpret_cast<const GLvoid*>(0));

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
			sizeof(MyVertex),
			reinterpret_cast<const GLvoid*>(sizeof(float[3]) + sizeof(float[2])));

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
			sizeof(MyVertex),
			reinterpret_cast<const GLvoid*>(sizeof(Vector3f)));

		glDrawElements(
			GL_TRIANGLES,
			terrain.GetNumberOfTriangles() * 3,
			GL_UNSIGNED_INT,
			0);

	}


	


	// Disable the vertex attributes (not necessary but recommended)
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);


	// Disable the shader program (not necessary but recommended)
	glUseProgram(0);

	// Lock the mouse at the center of the screen
	//glutWarpPointer(MouseX, MouseY);

	// Swap the frame buffers (off-screen rendering)
	glutSwapBuffers();
}

// Called at regular intervals (can be used for animations)
void Programm::idle() {
	clock_t now = clock();
	if (rotatebuilding) {
		buildingangle += (double)(now - lasttime) / CLOCKS_PER_SEC / 5;
		if (buildingangle >= 1.0)
			buildingangle -= floor(buildingangle);
	}

	if (movecamera) {
		cameracoef += (double)(now - lasttime) / CLOCKS_PER_SEC / 25;
		if (cameracoef >= 1.0)
			cameracoef -= floor(cameracoef);
		if (movecamera2) {
			Cam.target = Cam.Evaluatepoint2(cameracoef);
			Vector3f temp = Cam.target.cross(Vector3f{ 0.0f,1.0f,0.0f });
			Cam.up = temp.cross(Cam.target);
		}Cam.position = Cam.Evaluatepoint(cameracoef);
	}

	difference += (double)(now- lasttime)/CLOCKS_PER_SEC /5;
	if (difference >= 1.0)
		difference -= floor(difference);

	//cout << difference << endl;
	lasttime = now;
	glutPostRedisplay();
}


// Called whenever a keyboard button is pressed (only ASCII characters)
void Programm::keyboard(unsigned char key, int x, int y) {
	Vector3f right;

	switch (tolower(key)) {
		// --- camera movements ---
	case 'w':
		Cam.position += Cam.target * 0.1f;
		break;
	case 'a':
		right = Cam.target.cross(Cam.up);
		Cam.position -= right * 0.1f;
		break;
	case 's':
		Cam.position -= Cam.target * 0.1f;
		break;
	case 'd':
		right = Cam.target.cross(Cam.up);
		Cam.position += right * 0.1f;
		break;
	case 'c':
		Cam.position -= Cam.up * 0.1f;
		break;
	case ' ':
		Cam.position += Cam.up * 0.1f;
		break;
	case 'r': // Reset camera status
		Cam.Reset();
		buildingangle = 0.0;
		directional = true;
		break;

	case 'i': //change perspective
		if (Cam.projection == Projections::Perspective) {
			Cam.projection = Projections::Orthogonal;
		}
		else
			Cam.projection = Projections::Perspective;
		break;

	case 'k': //change shader
		if (shadertype == Shadertype::FragmentIllumination) {
			if (LoadShaders("shader.v.glsl", "shader.f.glsl")) {
				shadertype = Shadertype::Vertexillumination;
				cout << "> done." << endl;
			}
			else {
				cout << "> not possible." << endl;
			}
		}
		else {
			if (LoadShaders("Vertexshader.glsl", "Fragmentshader.glsl")) {
				cout << "> done." << endl;
				shadertype = Shadertype::FragmentIllumination;
			}
			else {
				cout << "> not possible." << endl;
			}
			
		}break;
		// --- utilities ---

	case 'p': // change to wireframe rendering
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'l': // change light option
		directional = !directional;
		break;

	case 'o': // change to polygon rendering
		glPolygonMode(GL_FRONT, GL_FILL);
		break;

	case 'g': // show the current OpenGL version
		cout << "OpenGL version " << glGetString(GL_VERSION) << endl;
		break;

	case 'm':  // terminate the application
		rotatebuilding = !rotatebuilding;
		break;

	case 'h':  // terminate the application
		if (movecamera) {
			movecamera2 = !movecamera2;
		}
		break; 

	case 'n':  // terminate the application
		movecamera = !movecamera;
		movecamera2 = true;
		cameracoef = 0;
		break;

	case 'b':  
		coloursbyheight = !coloursbyheight;
		break;
	case 'q':  // terminate the application
		exit(0);
		break;

	case '+':
		texturevsmaterialindex += 0.02;
		texturevsmaterialindex = min(texturevsmaterialindex, 1.0);
		break;

	case '-':
		texturevsmaterialindex -= 0.02;
		texturevsmaterialindex = max(texturevsmaterialindex, 0.0);
		break;

	case '1':
		terrain.Reset();
		InitTerrain("terrain\\bergen_1024x918.bin");
		break;

	case '2':
		terrain.Reset();
		InitTerrain("terrain\\bergen_2048x1836.bin");
		break;

	case '3':
		terrain.Reset();
		InitTerrain("terrain\\bergen_3072x2754.bin");
		break;
	}

	// redraw
	glutPostRedisplay();
}

// Called whenever a special keyboard button is pressed
void Programm::special(int key, int x, int y) {
	Vector3f right;

	switch (key) {

		// --- camera movements ---
	case GLUT_KEY_PAGE_UP:	// Increase field of view
		Cam.fov = min(Cam.fov + 1.f, 179.f);
		break;
	case GLUT_KEY_PAGE_DOWN:	// Decrease field of view
		Cam.fov = max(Cam.fov - 1.f, 1.f);
		break;

		// --- utilities ---
	case GLUT_KEY_F5:	// Reload shaders
		cout << "Re-loading shaders..." << endl;

		if (LoadShaders("shader.v.glsl", "shader.f.glsl")) {
			cout << "> done." << endl;
		}
		else {
			cout << "> not possible." << endl;
		}
		break;
	}
	// redraw
	glutPostRedisplay();
}

// Called whenever a mouse event occur (press or release)
void Programm::mouse(int button, int state, int x, int y) {
	// Store the current mouse status
	MouseButton = button;

	// Instead of updating the mouse position, lock it at the center of the screen
	//MouseX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	//MouseY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	//glutWarpPointer(MouseX, MouseY);aaa

	MouseX = x;
	MouseY = y;
}

// Called whenever the mouse is moving while a button is pressed
void Programm::motion(int x, int y) {
	if (MouseButton == GLUT_RIGHT_BUTTON) {
		Cam.position += Cam.target * 0.015f * (MouseY - y);
		Cam.position += Cam.target.cross(Cam.up) * 0.0015f * (x - MouseX);
		MouseX = x; // Store the current mouse position
		MouseY = y;
	}
	if (MouseButton == GLUT_MIDDLE_BUTTON) {
		Cam.zoom = std::max(0.001f, Cam.zoom + 0.0153f * (y - MouseY));
		MouseX = x; // Store the current mouse position
		MouseY = y;
	}
	if (MouseButton == GLUT_LEFT_BUTTON) {
		Matrix4f ry, rr;

		// "horizontal" rotation
		ry.rotate(0.05f * (MouseX - x), Vector3f(0, 1, 0));
		Cam.target = ry * Cam.target;
		Cam.up = ry * Cam.up;

		// "vertical" rotation
		rr.rotate(0.05f * (MouseY - y), Cam.target.cross(Cam.up));
		Cam.up = rr * Cam.up;
		Cam.target = rr * Cam.target;

		MouseX = x; // Store the current mouse position
		MouseY = y;
	}

	// Redraw the scene
	glutPostRedisplay();
}
