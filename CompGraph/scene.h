
// util.c
GLuint make_texture(const char *filename);
GLuint CompileAndLinkShaders(const char *vertex_code, const char *fragment_code);
GLuint LoadCompileAndLinkShaders(char *vs, char *fs);



// matrix.c
typedef double Matrix4[4][4];		// a matrix is a 4 by 4 array of doubles

void Matrix4Multiply (Matrix4 d, Matrix4 a, Matrix4 b);
void printMatrix4(Matrix4 d);
void copyMatrix4(Matrix4 d, Matrix4 a);
void MakeTranslateMatrix(Matrix4 d, double dx, double dy, double dz);
void MakeRotateXMatrix(Matrix4 d, double a);
void MakeRotateYMatrix(Matrix4 d, double a);
void MakeRotateZMatrix(Matrix4 d, double a);
void MakeViewPosDir(Matrix4 d, double cx, double cy, double cz, double elev, double ang);
void MakePerspective(Matrix4 d, double fovy, double a, double n, double f);
void Matrix4toGLfloat(GLfloat *d, Matrix4 s);

// readCollada.c

typedef struct {
	int npars;
	int parType;

	int nsurfs;
	char **mats;
	GLfloat **vals;
	int *nverts;
} geometry;

typedef struct sceneGraph_tag {
	geometry *g;
	char *name;
	
	struct sceneGraph_tag *next;
	struct sceneGraph_tag *prev;
	struct sceneGraph_tag *children;
	struct sceneGraph_tag *lastchild;
	struct sceneGraph_tag *parent;
} sceneGraph;

typedef struct {
	geometry *g;
	int nshapes;
	
	sceneGraph *root;
	
} scene;

scene *readScene(char *name);

