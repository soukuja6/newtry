
#include "Header.h"
using namespace std;

Programm program;
// --- OpenGL callbacks ---------------------------------------------------------------------------
void display() { program.display(); };
void keyboard(unsigned char a, int b, int c) { program.keyboard(a, b, c); };
void idle() { program.idle(); };
void mouse(int a, int b, int c, int d) { program.mouse(a, b, c, d); };
void motion(int a, int b) { program.motion(a, b); };
void special(int key, int x, int y) { program.special(key, x, y); }

// --- Other methods ------------------------------------------------------------------------------



						// --- main() -------------------------------------------------------------------------------------
						/// The entry point of the application





int main(int argc, char **argv) {

	// Initialize glut and create a simple window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(300, 50);
	glutCreateWindow("OpenGL Tutorial");

	// Initialize OpenGL callbacks
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	// Initialize glew (must be done after glut is initialized!)
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		cerr << "Error initializing glew: \n"
			<< reinterpret_cast<const char*>(glewGetErrorString(res)) << endl;
		return -1;
	}

	// Initialize program variables
	// OpenGL
	//glutSetCursor(GLUT_CURSOR_CROSSHAIR); // hide the cursor
	glClearColor(0.1f, 0.3f, 0.1f, 0.0f); // background color
	//glEnable(GL_DEPTH_TEST);	        // enable depth ordering
	//glEnable(GL_CULL_FACE);		        // enable back-face culling
	//glFrontFace(GL_CCW);		        // vertex order for the front face
	//glCullFace(GL_BACK);		        // back-faces should be removed

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);   // draw polygons as wireframe

										// Transformation
	program.Translation.set(0.0f, 0.0f, 0.0f);
	program.Scaling = 1.0f;

	// Shaders & mesh
	
	program.skybox.Init();
	if (!program.skybox.Load("shader.v.glsl", "shader.f.glsl") || !program.skybox.Loadtextures())
		return -1;

	
	if (!program.LoadShaders("Vertexshader.glsl", "Fragmentshader.glsl") ||  !program.InitTerrain("terrain\\bergen_1024x918.bin") || !program.InitMesh("building\\building.obj") )
		return -1;


	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);          // We'll Clear To The Color Of The Fog ( Modified )
	

	// Start the main event loop
	glutMainLoop();

	// clean-up before exit
	for (auto && texture : program.building.maintexture) {
		if (texture.TextureData != nullptr)
			free(texture.TextureData);
	}

	for (auto && texture : program.cinematexture) {
		if (texture.TextureData != nullptr)
			free(texture.TextureData);
	}

	for (auto && texture : program.dragon.maintexture) {
		if (texture.TextureData != nullptr)
			free(texture.TextureData);
	}

	if (program.bumptexture.TextureData != nullptr)
		free(program.bumptexture.TextureData);

	program.terrain.Reset();
	return 0;
}




// ************************************************************************************************
// *** Other methods implementation ***************************************************************



  

