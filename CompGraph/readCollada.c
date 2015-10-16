#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <glew.h>
//#include <GL/glut.h>

#include "scene.h"

#define DEBUG_PRINT 1

void applySubElementType(xmlNode * a_node, char *type, void (*act)(xmlNode *n, void *context), void *context)
// applies function {act()} to all sub-elements of {a_node}, of type {type}.
// to allow parameters to be passed, the data contained in {context} is passed
// to all the calls to {act()}
{
      int i=2,k;
    xmlNode *cur_node = NULL;

    for (cur_node = a_node, k=0; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
                           //printf("%s\t%s\n",cur_node->name,type);
                           //strcmp: 0:stringhe uguali, <0:s1<s2 , >0:s1>s2
        	if((type == NULL) || (strcmp((char *)cur_node->name, type) == 0)) {
                     
	        	act(cur_node, context);
        	}
        //printf("----%s----\n",cur_node->name);
        }
        
        /*
        printf("Cur_Node: %s\tType: %s\n",cur_node->name, type);
     if(cur_node->children!=NULL)
          printf("Figlio: %s\n",cur_node->children->name);
     if(cur_node->next!=NULL)
          printf("NExt: %s\n",cur_node->next->name);
     if(cur_node->prev!=NULL)
          printf("Prev: %s\n",cur_node->prev->name);
     if(cur_node->last!=NULL)     
          printf("Last: %s\n",cur_node->last->name);
          printf("Content:''%s''\n\n",cur_node->content);
        */
        
        //printf("Chiamata %s\t%d\n",cur_node->name,i);
        applySubElementType(cur_node->children, type, act, context);
        i++;
        k++;
    }
    
}




typedef struct {
	int n;
} countContext;

void count(xmlNode *n, void *context) {
     //printf("%s\n",n->name);
     printf("count\n");
	countContext *c = (countContext *)context;
	//printf("%d\n",c->n);
	c->n++;
}








typedef struct {
	char *id;
	int n;
	double *v;
} fillArrayContext;

#define MAX_TEMP_NUM 80

void fillArray(xmlNode *n, void *context) {
	char tempNum[MAX_TEMP_NUM];
	char *p;
	int i, j;
	xmlNode *cur_node = NULL;
	fillArrayContext *c = (fillArrayContext *)context;
	
	if((c->id == NULL) || (strcmp((char *)xmlGetProp(n, (xmlChar *)"id"), c->id) == 0)) {
		if(c->n == 0) {
            printf("%s, %s\n", c->id, (char *)xmlGetProp(n, (xmlChar *)"count") );
			c->n = atoi((char *)xmlGetProp(n, (xmlChar *)"count"));
		}
		c->v = (double *)calloc(c->n, sizeof(double));	
	    for (cur_node = n->children; cur_node; cur_node = cur_node->next) {
	        if (cur_node->type == XML_TEXT_NODE) {
				p = (char *)cur_node->content;
				i = 0;   j = 0;
				while(*p != '\0') {
					if((*p != ' ') && (i < MAX_TEMP_NUM-1)) {
						tempNum[i] = *p;
						i++;
					} else {
						tempNum[i] = '\0';
						i = 0;
						if(j < c->n) {
							c->v[j] = atof(tempNum);
							j++;
						}
					}
					p++;
				}
				if(i > 0) {
					tempNum[i] = '\0';
					if(j < c->n) {
						c->v[j] = atof(tempNum);
						j++;
					}
				}
	        }
	    }
	    for(; j < c->n; j++) {
	    	c->v[j] = 0.0;
	    }
	}
}



typedef struct {
	geometry *g;
	int nindex;
	int valsize[3];
	int Ninvals[3];
	double *invals[3];
	int i;
} polylistContext;

void readPolylist(xmlNode *n, void *context) {
	polylistContext *c = (polylistContext *)context;
	fillArrayContext fc;
	int i, j, k, l, m, ti, pi, si, di, totidx, ntri;
	double *vcounts, *vi;

	geometry *g = c->g;

	char *mat = (char *)xmlGetProp(n, (xmlChar *)"material");
	int npoly = atoi((char *)xmlGetProp(n, (xmlChar *)"count"));
	
	g->mats[c->i] = mat;
#ifdef DEBUG_PRINT
	if(mat != NULL) {
		printf("Npoly: %d,\tMaterial: >%s<\n", npoly, mat);
	} else {
		printf("Npoly: %d,\tNo material assigned\n", npoly);
	}
#endif
	fc.id = NULL; fc.n = npoly;
	applySubElementType(n->children, "vcount", fillArray, &fc);
	vcounts = fc.v;
	
	totidx = 0;
	ntri = 0;
	for(i = 0; i < npoly; i++) {
		totidx += (int)fc.v[i];
		ntri += (int)fc.v[i] - 2;
	}
#ifdef DEBUG_PRINT
printf("Geometry:%d, Tot idx: %d\tTot tri: %d\n", c->i, totidx, ntri);	
#endif

	g->nverts[c->i] = ntri * 3;
	g->vals[c->i] = (float *)calloc(ntri * 3 * g->npars, sizeof(float));

	fc.id = NULL; fc.n = totidx * c->nindex;
	applySubElementType(n->children, "p", fillArray, &fc);
	vi = fc.v;
	
	di = 0;
	pi = 0;
	for(i = 0; i < npoly; i++) {
		for(j = 2; j < (int)vcounts[i]; j++) {
			for(k = 0; k < 3; k++) {
				ti = (k == 0 ? 0 : (k == 1 ? j - 1 : j));
				for(l = 0; l < c->nindex; l++) {
					for(m = 0; m < c->valsize[l]; m++) {
						si = (int)vi[(pi + ti) * c->nindex + l] * c->valsize[l] + m;
						if(si < c->Ninvals[l]) {
							g->vals[c->i][di] = c->invals[l][si];
						} else {
							printf("\n ERROR! VERTEX OUT OF BOUNDS! %d >= %d\n", si, c->Ninvals[l]);
						}
						di++;
					}
				}
			}
		}
		pi += (int)vcounts[i];
	}

#ifdef DEBUG_PRINT
printf("\n\n");	
#endif
	free(vcounts);
	free(vi);
	
	c->i++;	// go to the next geometry
}



void ZupToYup(fillArrayContext *fc)
{
	int i;
	double tmp;
	
	for(i = 0; i < fc->n; i+=3) {
		tmp = fc->v[i + 1];
		fc->v[i + 1] = fc->v[i + 2];
		fc->v[i + 2] = -tmp;
	}
}




#define MAX_LOCAL_BUF 256
void fillGeometry(xmlNode *n, geometry *g) {
	char localBuf[MAX_LOCAL_BUF];
	fillArrayContext fc;
	countContext cc;
	polylistContext plc;
	
	double *pos, *norm, *uv;
	int Npos, Nnorm, Nuv;
	
	//printf("%s\n",n->name);
	
	g->parType = 0;
	g->npars = 0;
	plc.nindex = 0;

#ifdef DEBUG_PRINT
printf("Reading mesh: %s\n", xmlGetProp(n, (xmlChar *)"id"));
#endif

	sprintf(localBuf, "%s-positions-array", xmlGetProp(n, (xmlChar *)"id"));
	//printf("%s\n",localBuf);
	fc.id = localBuf; fc.n = 0;
	applySubElementType(n, "float_array", fillArray, &fc);
	ZupToYup(&fc);	// converts from Z-up to Y-up
	Npos = fc.n; pos = fc.v;
	if(Npos > 0) {
		g->parType |= 1;
		g->npars += 3;
		plc.valsize[plc.nindex] = 3;
		plc.invals[plc.nindex] = pos;
		plc.Ninvals[plc.nindex] = Npos;
		plc.nindex ++;
	}
#ifdef DEBUG_PRINT
printf("%d vertices\n", Npos/3);
#endif

	sprintf(localBuf, "%s-normals-array", xmlGetProp(n, (xmlChar *)"id"));
	//printf("%s\n",localBuf);
	fc.id = localBuf; fc.n = 0;
	applySubElementType(n, "float_array", fillArray, &fc);
	ZupToYup(&fc);	// converts from Z-up to Y-up
	Nnorm = fc.n; norm = fc.v;
	if(Nnorm > 0) {
		g->parType |= 2;
		g->npars += 3;
		plc.valsize[plc.nindex] = 3;
		plc.invals[plc.nindex] = norm;
		plc.Ninvals[plc.nindex] = Nnorm;
		plc.nindex ++;
	}
#ifdef DEBUG_PRINT
printf("%d normals\n", Nnorm/3);
#endif

	sprintf(localBuf, "%s-map-0-array", xmlGetProp(n, (xmlChar *)"id"));
	fc.id = localBuf; fc.n = 0;
	applySubElementType(n, "float_array", fillArray, &fc);
	Nuv = fc.n; uv = fc.v;
	if(Nuv > 0) {
		g->parType |= 4;
		g->npars += 2;
		plc.valsize[plc.nindex] = 2;
		plc.invals[plc.nindex] = uv;
		plc.Ninvals[plc.nindex] = Nuv;
		plc.nindex ++;
	}
#ifdef DEBUG_PRINT
printf("%d UV (first set)\n", Nuv/2);
#endif

#ifdef DEBUG_PRINT
printf("par code: 0x%x, npars: %d, nindex: %d\n", g->parType, g->npars, plc.nindex);
#endif

	cc.n = 0;
    applySubElementType(n->children, "polylist", count, &cc);
    g->nsurfs = cc.n;
#ifdef DEBUG_PRINT
printf("number of surfaces: %d\n", g->nsurfs);
#endif
	g->nverts = (int *)   calloc(g->nsurfs, sizeof(int));
	g->mats   = (char **) calloc(g->nsurfs, sizeof(char *));
	g->vals   = (float **)calloc(g->nsurfs, sizeof(float *));
	printf("%d , %s , %f\n", g->nverts, g->mats, g->vals);
	plc.g = g;
	plc.i = 0;
	applySubElementType(n->children, "polylist", readPolylist, &plc);

#ifdef DEBUG_PRINT
printf("\n");
#endif

	if(Npos > 0) free(pos);
	if(Nnorm > 0) free(norm);
	if(Nuv > 0) free(uv);
}


typedef struct {
	geometry *g;
	int n;
} geomteryContext;

void readGeometry(xmlNode *n, void *context) {
          printf("readGeometry\n%s\n",n->name);
	geomteryContext *c = (geomteryContext *)context;
	printf("GeometryContext (N): %d\n",c->n);
	fillGeometry(n, &c->g[c->n]);
	c->n++;
}



void makeSceneGraph(sceneGraph *node, xmlNode * n)
{
    xmlNode *cur_node = NULL;
    sceneGraph *newNode;
    sceneGraph *recur;

    for (cur_node = n; cur_node; cur_node = cur_node->next) {
		recur = node;
        if (cur_node->type == XML_ELEMENT_NODE) {
        	if(strcmp((char *)cur_node->name, "node") == 0) {
#ifdef DEBUG_PRINT        		
printf("%s in %s\n", xmlGetProp(cur_node, (xmlChar *)"id"), node->name);
#endif
        		newNode = (sceneGraph *)malloc(sizeof(sceneGraph));
				newNode->name = (char *)xmlGetProp(cur_node, (xmlChar *)"id");
        		newNode->parent = node;
        		if(node->children == NULL) {
        			node->children = node->lastchild = newNode;
        			printf("no children: %s\n", node->children->name);
        			newNode->prev = newNode->next = NULL;
        		} else {
                    printf("si children: %s\n", node->children->name);
                    printf("Node: %s\n", node->name);
                    printf("NewNode: %s\n", newNode->name);
                    printf("PARENT: %s\n\n", newNode->parent->name);
        			newNode->next = NULL;
        			newNode->prev = node->lastchild;
        			newNode->prev->next = newNode;
        			node->lastchild = newNode;
        		}
        		newNode->children = newNode->lastchild = NULL;
        		recur = newNode;
        	}
        }
        makeSceneGraph(recur, cur_node->children);
    }
}


scene *readScene(char *name) {
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    countContext cc;
    geomteryContext gc;

	scene *s;
	sceneGraph *root;
	
	s = (scene *)malloc(sizeof(scene));

    /*parse the file and get the DOM (Document Object Model) */
    doc = xmlReadFile(name, NULL, 0);

    if (doc == NULL) {
        printf("error: could not parse file %s\n", name);
        exit(0);
    }
    
    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);
    //Stampa: COLLADA
    //printf("%s",root_element->name);

	// Reads the geometries
	cc.n = 0;
	//printf("Chiamata 1");
    applySubElementType(root_element, "geometry", count, &cc);
#ifdef DEBUG_PRINT
printf("There are %d <geometry> nodes\n", cc.n);
#endif
    gc.n = 0;
    gc.g = (geometry *)calloc(cc.n, sizeof(geometry));
    applySubElementType(root_element, "geometry", readGeometry, &gc);
    s->g = gc.g;
    s->nshapes = gc.n;
    
    
    // Reads the scene graph
	root = (sceneGraph *)malloc(sizeof(sceneGraph));
	s->root = root;
	root->g = NULL;
	root->parent = root->children = root->lastchild = root->next = root->prev = NULL;
	root->name = "_root";
	makeSceneGraph(root, root_element);

    /*free the document */
    xmlFreeDoc(doc);
    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();
    
    return s;
}

