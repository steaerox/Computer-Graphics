#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <glew.h>
//#include <GL/glut.h>

#include "scene.h"


/*********************************************
**        Simple Matrix procedures          **
*********************************************/



void Matrix4Multiply (Matrix4 d, Matrix4 a, Matrix4 b)
// Perform matrix product = {d = a * b;}, where {d}, {a} and {b} are matrices
{
	d[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0] + a[0][3] * b[3][0]; 
	d[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1] + a[0][3] * b[3][1]; 
	d[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2] + a[0][3] * b[3][2]; 
	d[0][3] = a[0][0] * b[0][3] + a[0][1] * b[1][3] + a[0][2] * b[2][3] + a[0][3] * b[3][3]; 
	d[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0] + a[1][3] * b[3][0]; 
	d[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1] + a[1][3] * b[3][1]; 
	d[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2] + a[1][3] * b[3][2]; 
	d[1][3] = a[1][0] * b[0][3] + a[1][1] * b[1][3] + a[1][2] * b[2][3] + a[1][3] * b[3][3]; 
	d[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0] + a[2][3] * b[3][0]; 
	d[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1] + a[2][3] * b[3][1]; 
	d[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2] + a[2][3] * b[3][2]; 
	d[2][3] = a[2][0] * b[0][3] + a[2][1] * b[1][3] + a[2][2] * b[2][3] + a[2][3] * b[3][3]; 
	d[3][0] = a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0] + a[3][3] * b[3][0]; 
	d[3][1] = a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1] + a[3][3] * b[3][1]; 
	d[3][2] = a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2] + a[3][3] * b[3][2]; 
	d[3][3] = a[3][0] * b[0][3] + a[3][1] * b[1][3] + a[3][2] * b[2][3] + a[3][3] * b[3][3]; 
};

void printMatrix4(Matrix4 d) {
// prints a matrix {d}
	printf("|%3.5g  %3.5g  %3.5g  %3.5g|\n", d[0][0], d[0][1], d[0][2], d[0][3]);
	printf("|%3.5g  %3.5g  %3.5g  %3.5g|\n", d[1][0], d[1][1], d[1][2], d[1][3]);
	printf("|%3.5g  %3.5g  %3.5g  %3.5g|\n", d[2][0], d[2][1], d[2][2], d[2][3]);
	printf("|%3.5g  %3.5g  %3.5g  %3.5g|\n\n", d[3][0], d[3][1], d[3][2], d[3][3]);
}

void copyMatrix4(Matrix4 d, Matrix4 a) {
// duplicates matrix {a} into {d}
	d[0][0] = a[0][0];	d[0][1] = a[0][1];	d[0][2] = a[0][2];	d[0][3] = a[0][3];
	d[1][0] = a[1][0];	d[1][1] = a[1][1];	d[1][2] = a[1][2];	d[1][3] = a[1][3];
	d[2][0] = a[2][0];	d[2][1] = a[2][1];	d[2][2] = a[2][2];	d[2][3] = a[2][3];
	d[3][0] = a[3][0];	d[3][1] = a[3][1];	d[3][2] = a[3][2];	d[3][3] = a[3][3];
}

/*********************************************
**       Transofrm Matrix procedures        **
*********************************************/

void MakeTranslateMatrix(Matrix4 d, double dx, double dy, double dz) {
// Create a transform matrix for a translation of ({dx}, {dy}, {dz}).
// The resulting matrix is stored in {d}
	d[0][0] = d[1][1] = d[2][2] = d[3][3] = 1.0;
	d[0][1] = d[0][2] = d[1][0] = d[1][2] =
	d[2][0] = d[2][1] = d[3][0] = d[3][1] = d[3][2] = 0.0;
	d[0][3] = dx;
	d[1][3] = dy;
	d[2][3] = dz;
}

void MakeRotateXMatrix(Matrix4 d, double a) {
// Create a transform matrix for a rotation of {a} along the X axis.
// The resulting matrix is stored in {d}
	double adeg = a * M_PI / 180.0;
	double c = cos(adeg);
	double s = sin(adeg);
	d[1][1] = d[2][2] = c;
	d[1][2] = -s;
	d[2][1] = s;
	d[0][0] = d[3][3] = 1.0;
	d[0][1] = d[0][2] = d[0][3] = 
	d[1][0] = d[1][3] = d[2][0] = d[2][3] =
	d[3][0] = d[3][1] = d[3][2] = 0.0;
}

void MakeRotateYMatrix(Matrix4 d, double a) {
// Create a transform matrix for a rotation of {a} along the Y axis.
// The resulting matrix is stored in {d}
	double adeg = a * M_PI / 180.0;
	double c = cos(adeg);
	double s = sin(adeg);
	d[0][0] = d[2][2] = c;
	d[0][2] = -s;
	d[2][0] = s;
	d[1][1] = d[3][3] = 1.0;
	d[0][1] = d[0][3] = d[2][1] = d[2][3] = 
	d[1][0] = d[1][2] = d[1][3] =
	d[3][0] = d[3][1] = d[3][2] = 0.0;
}

void MakeRotateZMatrix(Matrix4 d, double a) {
// Create a transform matrix for a rotation of {a} along the Z axis.
// The resulting matrix is stored in {d}
	double adeg = a * M_PI / 180.0;
	double c = cos(adeg);
	double s = sin(adeg);
	d[0][0] = d[1][1] = c;
	d[1][0] = -s;
	d[0][1] = s;
	d[2][2] = d[3][3] = 1.0;
	d[0][2] = d[0][3] = d[1][2] = d[1][3] =
	d[2][0] = d[2][1] = d[2][3] =
	d[3][0] = d[3][1] = d[3][2] = 0.0;
}



/*********************************************
**          View Matrix procedures          **
*********************************************/

void MakeViewPosDir(Matrix4 d, double cx, double cy, double cz, double elev, double ang) {
// Creates in {d} a view matrix. The camera is centerd in ({cx}, {cy}, {cz}).
// It looks {ang} degrees on y axis, and {elev} degrees on the x axis.

	Matrix4 T, Rx, Ry, tmp;
	MakeTranslateMatrix(T, -cx, -cy, -cz);
	MakeRotateXMatrix(Rx, -elev);
	MakeRotateYMatrix(Ry, -ang);
	
	// Since we are deailing with transformations, we can use the "T" version of the product
	Matrix4Multiply(tmp, Ry, T);
	Matrix4Multiply(d, Rx, tmp);
}


// FOR EXERCISE: ADD THE PROCEDURE FOR THE "LookAt()" MATRIX


/*********************************************
**       Projection Matrix procedures       **
*********************************************/

// Only the procedures for the perspective matrix will be completed.
// For what concerns parallel projection, only the procedure for constructing
// the matrix will be presented. To correctly implement parallel projections,
// also the descriptions of the sides of the frustum, and the direction of
// projection rays should be computed and stored somewhere in the rendering pipeline.

void MakePerspective(Matrix4 d, double fovy, double a, double n, double f) {
// creates the perspective projection matrix. The matrix is stored in {d}.
// {fovy} contains the vertical field-of-view in degree. {a} is the aspect ratio.
// {n} is the distance of the near plane,piano con la coordinata z Minimina (+ vicino all'osservatore)
// {f} is the far plane (piano a coordinata z Massima + lontano dall'osservatore

// La coordinata z varia tra [n , f] (di solito -1 e 1) Qua valgono 0.1 e 1000
	double halfFovyRad = fovy * M_PI / 360.0;	// stores {fovy/2} in radiants
	double ct = 1.0 / tan(halfFovyRad);			// cotangent of {fov/2}
	
	d[0][0] = ct / a;
	d[1][1] = ct;
	d[2][2] = (f + n) / (n - f);
	d[2][3] = 2.0 * f * n / (n - f);
	d[3][2] = -1.0;
	d[0][1] = d[0][2] = d[0][3] = 
	d[1][0] = d[1][2] = d[1][3] = 
	d[2][0] = d[2][1] =
	d[3][0] = d[3][1] = d[3][3] = 0.0;	
	
}


void Matrix4toGLfloat(GLfloat *d, Matrix4 s) {
     d[ 0] = s[0][0];     d[ 1] = s[0][1];      d[ 2] = s[0][2];     d[ 3] = s[0][3];
     d[ 4] = s[1][0];     d[ 5] = s[1][1];      d[ 6] = s[1][2];     d[ 7] = s[1][3];
     d[ 8] = s[2][0];     d[ 9] = s[2][1];      d[10] = s[2][2];     d[11] = s[2][3];
     d[12] = s[3][0];     d[13] = s[3][1];      d[14] = s[3][2];     d[15] = s[3][3];
     
}
