#ifdef __APPLE__  // include Mac OS X verions of headers
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else // non-Mac OS X operating systems
#  include <GL/glew.h>
//#  include <GL/glut.h>
#  include <GL/freeglut.h>
#  include <GL/freeglut_ext.h>
#endif  // __APPLE__

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "shader.h"

GLfloat *Vertices = NULL;
GLuint *Indices = NULL;

int VertexNum;
int FaceNum;

// OpenGL ----

Shader myShader;
GLuint VAO;

int screenWidth;
int screenHeight;

void readSMF(const char *name){
	FILE *fin = NULL;
	fin = fopen(name,"r");
	if(fin == NULL){
		printf("Error Opening the File\n");
		fclose(fin); exit(1);
	}

	if( fscanf(fin," # %d %d",&VertexNum,&FaceNum) < 2) {
		printf("Error Reading the File\n");
		fclose(fin); exit(1);
	}

	Vertices = new GLfloat [ VertexNum * 3 ];
	if(Vertices == NULL) {
		printf("Error Allocating Memory\n");
		fclose(fin); exit(1);
	}
	Indices = new GLuint [ FaceNum * 3 ];
	if(Indices == NULL) {
		printf("Error Allocating Memory\n");
		fclose(fin); exit(1);
	}

	for(int i=0;i<VertexNum;i++)
		if( fscanf(fin," v %f %f %f",&Vertices[i*3], &Vertices[i*3 +1], &Vertices[i*3 +2]) < 3){
			printf("Error Reading the File\n");
			fclose(fin); exit(1);
		}

	int ta,tb,tc;
	for(int i=0;i<FaceNum;i++){
		if( fscanf(fin," f %d %d %d",&ta,&tb,&tc) < 3){
			printf("Error Reading the File\n");
			fclose(fin); exit(1);
		}
		Indices[i*3] = ta-1;
		Indices[i*3 +1] = tb-1;
		Indices[i*3 +2] = tc-1;
	}

	fclose(fin);
}

inline float min(float a,float b) { return a>b? b:a; }
inline float max(float a,float b) { return a>b? a:b; }

void checkMinMax(){
	float Min[3] = {100,100,100};
	float Max[3] = {0,0,0};

	for(int i=0;i<VertexNum;i++)
		for(int j=0;j<3;j++){
			Min[j] = min(Min[j], Vertices[i*3 + j]);
			Max[j] = max(Max[j], Vertices[i*3 + j]);
		}

	for(int j=0;j<3;j++){
		printf("%c : ",'x' + j);
		printf("Min : %f ; Max : %f\n",Min[j],Max[j]);
	}
}

void printSMF(){
	printf("# %d %d\n",VertexNum,FaceNum);
	for(int i=0;i<VertexNum;i++)
		printf("v %f %f %f\n",Vertices[i*3],Vertices[i*3 +1],Vertices[i*3 +2]);
	for(int i=0;i<FaceNum;i++)
		printf("f %d %d %d\n",Indices[i*3]+1,Indices[i*3 +1]+1,Indices[i*3 +2]+1);
}


void setUpShader(){

	myShader = Shader("vshader.glsl","fshader.glsl");
	myShader.Use();

	// ---------------------------------
    GLuint VBO,EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);
    // Bind our Vertex Array Object first, then bind and set our buffers and pointers.
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * VertexNum * 3 , Vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * FaceNum * 3, Indices, GL_STATIC_DRAW);

    GLuint position = glGetAttribLocation(myShader.Program, "vPosition" ); 
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(position);

    // Unbind tileVAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
    glBindVertexArray(0); 
}

void keyboard( unsigned char key, int x, int y ){
    switch( key ) {
        case 033: // Escape Key
        case 'q': case 'Q':
            exit( EXIT_SUCCESS );
            break;
    }
}

void reshape( int width, int height ){ // Update viewport
    screenWidth = width;
    screenHeight = height;

    glViewport( 0, 0, width, height );
}

void display(void){ // Render

    // Clear the color buffer and depth buffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // Draw 
    // --------------------------------------------------------------
    myShader.Use();
    glBindVertexArray(VAO);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES,FaceNum * 3, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    // Update screen
    glutPostRedisplay();
    glutSwapBuffers();
}


int main(int argc, char **argv){
	if(argc < 2) {
		printf("Need to specify the SMF file to read\n");
		exit(1);
	}

	readSMF(argv[1]);
	checkMinMax();
	//printSMF();

    // Create window
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( screenWidth = 1300, screenHeight = 1000 );
    glutCreateWindow( "SMF" );

    // Initialize GLEW to setup the OpenGL Function pointers
    glewExperimental = GL_TRUE;
    glewInit(); 

    // Set OpenGL environment
    glEnable(GL_DEPTH_TEST);
    //glutSetCursor(GLUT_CURSOR_NONE);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    // Initialize and calculate graphics data
    setUpShader();
    //gameInit();

    // Bind GLUT callback functions
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutReshapeFunc( reshape );

    // Start main loop
    glutMainLoop();


	return 0;
}