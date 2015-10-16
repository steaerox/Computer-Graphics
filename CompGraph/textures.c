#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <glew.h>
//#include <GL/glut.h>

#include "scene.h"



// rotation speed
#define DELTA_ANG 2
// movement speed
#define DELTA_MOVE 2

// positions of the camera
double px =  0.0;
double py =  2.0;
double pz =  5.0;
double ang = 0.0;
double elev = 0.0;

void processNormalKeys(unsigned char key, int x, int y) {
// perform rotations
	if(key==('j') || key==('J')) {
		ang -= DELTA_ANG;
	} else if(key==('l') || key==('L')) {
		ang += DELTA_ANG;
    } else if(key==('i') || key==('I')) {
   		elev += DELTA_ANG;
	} else if(key==('k') || key==('K')) {
   		elev -= DELTA_ANG;
   	// perform movements
	} else if(key==('a') || key==('A')) {
   		px -= DELTA_MOVE * cos(ang * M_PI / 180.0);
   		pz -= DELTA_MOVE * sin(ang * M_PI / 180.0);
	} else if(key==('d') || key==('D')) {
   		px += DELTA_MOVE * cos(ang * M_PI / 180.0);
   		pz += DELTA_MOVE * sin(ang * M_PI / 180.0);
	} else if(key==('w') || key==('W')) {
   		px += DELTA_MOVE * sin(ang * M_PI / 180.0);
   		pz -= DELTA_MOVE * cos(ang * M_PI / 180.0);
	} else if(key==('s') || key==('S')) {
   		px -= DELTA_MOVE * sin(ang * M_PI / 180.0);
   		pz += DELTA_MOVE * cos(ang * M_PI / 180.0);
	} else if(key==('r') || key==('R')) {
   		py += DELTA_MOVE;
	} else if(key==('f') || key==('F')) {
   		py -= DELTA_MOVE;
    } else if(key == 27) {
        exit(0);
    }
}

void changeSize(int w, int h) {
    glViewport(0, 0, w, h);
}






// An array of 3 vectors which represents 3 vertices
static const GLfloat g_vertex_buffer_data[] = {
   -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   0.0f,  1.0f, 0.0f,
};

// This will identify our vertex buffer
GLuint *vertexbuffer;

// This will identify the shaders
GLuint programID[3];

// The Uniform (the parameter) passed to the vertex shader to include the
// world, view and projection matrices product
GLuint MatrixID[3];

// The Uniform (the parameter) passed to the fragment shader to specify the
// pixel color
GLuint fColorID[3];

// uniform for the specular color of the material
GLuint fSpecColorID[3];

// uniform for the specular color of the material
GLuint fSpecPowerID[3];

// uniform for the light color
GLuint lColorID[3];

// uniform for the light direction
GLuint lDirID[3];

// uniform for the ambient light term
GLuint lAmbID[3];

// this is the index for the eye position
GLuint EyePosID[3];

// this is the index for the texture sampler
GLuint texUnifID[3];

// this is the index for the position among the parameters
GLuint posParID[3];

// this is the index for the normal among the parameters
GLuint normParID[3];

// this is the index for the normal among the parameters
GLuint uvParID[3];

// the scene file
scene *scn;


// IDs of the texture
GLuint *textureIDs;

#define OBJS_WITH_TEXTURE 3
struct {
	char *tex;
	char *geo;
} texGeoName[OBJS_WITH_TEXTURE] = {
	{"auto.tga", "auto1"},
	{"terrain.tga", "terrain1"},
	{"building4.tga", "building41"}
};

#define DAE_FILE "paperCity2012e.dae"
//#define DAE_FILE "paperCity2012ePa - Copia.dae"
//#define DAE_FILE "hover_tank.dae"

void initScene() {
	int i,j;
     
    // Initialize shaders
	programID[0] = LoadCompileAndLinkShaders("lightP.vs", "lightP.fs");
	programID[1] = LoadCompileAndLinkShaders("texP.vs", "texP.fs");
	
	// gets indices of the uniforms
	for(i = 0; i < 2; i++) {
		MatrixID[i]     = glGetUniformLocation(programID[i], "p_matrix");
		fColorID[i]     = glGetUniformLocation(programID[i], "f_color");
		fSpecColorID[i] = glGetUniformLocation(programID[i], "f_spec_color");
		fSpecPowerID[i] = glGetUniformLocation(programID[i], "f_spec_power");
		lColorID[i]     = glGetUniformLocation(programID[i], "l_color");
		lDirID[i]       = glGetUniformLocation(programID[i], "l_dir");
		lAmbID[i]       = glGetUniformLocation(programID[i], "l_amb");
		EyePosID[i]     = glGetUniformLocation(programID[i], "eye_pos");
		texUnifID[i]     = glGetUniformLocation(programID[i], "texture");
		// gets indices of the parameters
		posParID[i]  = glGetAttribLocation(programID[i], "position");
		normParID[i] = glGetAttribLocation(programID[i], "normal");
		uvParID[i] = glGetAttribLocation(programID[i], "uv");
	}

	// load the scene
	scn = readScene(DAE_FILE);
	
	// allocate the memory for the vertex buffers
	vertexbuffer = (GLuint *)calloc(scn->nshapes, sizeof(GLuint));
    // Generate 1 buffer per geometric shape, put the resulting identifier in vertexbuffer
    glGenBuffers(scn->nshapes, vertexbuffer);
     
    for(i = 0; i < scn->nshapes; i++) {
	    // The following commands will talk about our 'vertexbuffer' buffer
	    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[i]);
	     
	    // Give our vertices to OpenGL.
	    glBufferData(GL_ARRAY_BUFFER,
	    			 sizeof(GLfloat) * scn->g[i].nverts[0] * scn->g[i].npars,
	    			 scn->g[i].vals[0], GL_STATIC_DRAW);
    }
    // color to clear the screen
	glClearColor(0.2,0.2,1.0,1.0);
	
	// enables Z Buffer
    glEnable(GL_DEPTH_TEST);
    
    // Loads the textures
    // calloc alloca un'area di memoria, la inizializza a 0, di dimensioni scn->nshapes * sizeof(GLuint)
    textureIDs = (GLuint *)calloc(scn->nshapes, sizeof(GLuint));
    for(i = 0; i < scn->nshapes; i++) {
    	for(j = 0; j < OBJS_WITH_TEXTURE; j++) {
    		if(strcmp(scn->g[i].mats[0], texGeoName[j].geo)==0) break;
    	}
    	if(j < OBJS_WITH_TEXTURE) {
    		textureIDs[i] = make_texture(texGeoName[j].tex);
    	} else {
    		textureIDs[i] = -1;
    	}
	}
}


// the projection matrix to be sent to the shader
GLfloat MVP[16];

void makeMVP()
// makes the World,View,Projection matrix to be sent to the shader
// with the current values of {px, py, pz, elev, ang}
{
    Matrix4 View;   // View matrix
    Matrix4 Proj;   // Projection matrix
    // since the World matrix is not used here, it is not included
    Matrix4 VP;     // World times view
    
    /*printMatrix4(View);
    printMatrix4(Proj);
    printMatrix4(VP);*/
    
    // a=4.0/3.0 Rapporto d'aspetto: Schermo 4/3
    //Proiezione Prospettica
	MakePerspective(Proj, 90.0, 4.0/3.0, 0.1, 1000);
    
    //View Matrix	
	MakeViewPosDir(View, px, py, pz, elev, ang);	
    Matrix4Multiply(VP, Proj, View);
    
    /*printMatrix4(View);
    printMatrix4(Proj);
    printMatrix4(VP);*/
    
    Matrix4toGLfloat(MVP, VP);
}

void renderScene(void) {    
	int i;
	int curShader;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    makeMVP(); // sends the World/View/Projection
    for(i = 0; i < scn->nshapes; i++) {
    	curShader = ((int)textureIDs[i] < 0 ? 0 : 1);
		// Use our shader
		glUseProgram(programID[curShader]);
		// Passes the world, view, projection matrix to the vertex shader
	    glUniformMatrix4fv(MatrixID[curShader], 1, GL_TRUE, MVP);
	    // Passes the other uniforms to the shader
	    	// material
	    glUniform3f(fColorID[curShader], 
	    					  (((i%7)+1)&1) ? 1.0 : 0.0,
	    					  (((i%7)+1)&2) ? 1.0 : 0.0,
	    					  (((i%7)+1)&4) ? 1.0 : 0.0);
	    glUniform3f(fSpecColorID[curShader], 1.0, 1.0, 1.0);
	    glUniform1f(fSpecPowerID[curShader], 16.0);
	    	// light
		glUniform3f(lColorID[curShader], 0.8, 0.8, 0.8);
		glUniform3f(lDirID[curShader], 1.0, -1.0, 1.0);
		glUniform1f(lAmbID[curShader], 0.2);
		// eye position
	    glUniform3f(EyePosID[curShader], px, py, pz);
		
	    // 1rst attribute buffer : vertices positions
	    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[i]);
	    glEnableVertexAttribArray(posParID[curShader]);
//printf("%d: %d ", i, vertexbuffer[i]);
	    glVertexAttribPointer(
	       posParID[curShader],           // attribute.
	       3,				   // size
	       GL_FLOAT,           // type
	       GL_FALSE,           // normalized?
	       (int)(scn->g[i].npars * sizeof(GLfloat)),   // stride
	       (void*)0            // array buffer offset
	    );
	    glEnableVertexAttribArray(normParID[curShader]);
	    glVertexAttribPointer(
	       normParID[curShader],           // attribute.
	       3,				   // size
	       GL_FLOAT,           // type
	       GL_FALSE,           // normalized?
	       (int)(scn->g[i].npars * sizeof(GLfloat)),   // stride
	       (void*)(3 * sizeof(GLfloat))            // array buffer offset
	    );
	    if(curShader == 1) {
		    glEnableVertexAttribArray(uvParID[curShader]);
		    glVertexAttribPointer(
		       uvParID[curShader],           // attribute.
		       2,				   // size
		       GL_FLOAT,           // type
		       GL_FALSE,           // normalized?
		       (int)(scn->g[i].npars * sizeof(GLfloat)),   // stride
		       (void*)(6 * sizeof(GLfloat))            // array buffer offset
		    );
		    
		    glBindTexture(GL_TEXTURE_2D, textureIDs[i]);
		    glActiveTexture(GL_TEXTURE0);
		    glUniform1i(texUnifID[curShader], 0);		    
	    }
//printf("%d ", (int)(scn->g[i].npars * sizeof(GLfloat)));
	     
	    // Draw the triangle !
	    glDrawArrays(GL_TRIANGLES, 0, scn->g[i].nverts[0]);
//printf("%d\n", scn->g[i].nverts[0]);
	    
	    glDisableVertexAttribArray(posParID[curShader]);
	    glDisableVertexAttribArray(normParID[curShader]);
	    if(curShader == 1) {
		    glDisableVertexAttribArray(uvParID[curShader]);
	    }
    }
//printf("\n");

	glutSwapBuffers();
}

int main(int argc, char *argv[]) {

	glutInit(&argc, argv);

	//glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(640, 480);
	glutCreateWindow("OpenGL - simple app");

	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(processNormalKeys);

	glewInit();

    initScene();

	// prints the key used by the program.
	printf("Keys are:\n");
	printf("\t a : move left\n");
	printf("\t d : move right\n");
	printf("\t w : move forward\n");
	printf("\t s : move backward\n");
	printf("\t r : move up\n");
	printf("\t f : move down\n");
	printf("\t j : turn left\n");
	printf("\t l : turn right\n");
	printf("\t i : turn forward\n");
	printf("\t k : turn backward\n");
	printf("\t ESC : quit\n");



	glutMainLoop();

    return 0;
}
