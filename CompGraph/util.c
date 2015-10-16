// This file has been composed with parts taken from a tutorial available on the net:
// www. ...

#include <stdlib.h>
#include <glew.h>
//#include <GL/glut.h>
#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scene.h"



static short le_short(unsigned char *bytes)
{
    return bytes[0] | ((char)bytes[1] << 8);
}

void *read_tga(const char *filename, int *width, int *height)
{
    struct tga_header {
       char  id_length;
       char  color_map_type;
       char  data_type_code;
       unsigned char  color_map_origin[2];
       unsigned char  color_map_length[2];
       char  color_map_depth;
       unsigned char  x_origin[2];
       unsigned char  y_origin[2];
       unsigned char  width[2];
       unsigned char  height[2];
       char  bits_per_pixel;
       char  image_descriptor;
    } header;
    int i, color_map_size, pixels_size;
    FILE *f;
    size_t read;
    void *pixels;

    f = fopen(filename, "rb");

    if (!f) {
        fprintf(stderr, "Unable to open %s for reading\n", filename);
        return NULL;
    }

    read = fread(&header, 1, sizeof(header), f);

    if (read != sizeof(header)) {
        fprintf(stderr, "%s has incomplete tga header\n", filename);
        fclose(f);
        return NULL;
    }
    if (header.data_type_code != 2) {
        fprintf(stderr, "%s is not an uncompressed RGB tga file\n", filename);
        fclose(f);
        return NULL;
    }
    if (header.bits_per_pixel != 24) {
        fprintf(stderr, "%s is not a 24-bit uncompressed RGB tga file\n", filename);
        fclose(f);
        return NULL;
    }

    for (i = 0; i < header.id_length; ++i)
        if (getc(f) == EOF) {
            fprintf(stderr, "%s has incomplete id string\n", filename);
            fclose(f);
            return NULL;
        }

    color_map_size = le_short(header.color_map_length) * (header.color_map_depth/8);
    for (i = 0; i < color_map_size; ++i)
        if (getc(f) == EOF) {
            fprintf(stderr, "%s has incomplete color map\n", filename);
            fclose(f);
            return NULL;
        }

    *width = le_short(header.width); *height = le_short(header.height);
    pixels_size = *width * *height * (header.bits_per_pixel/8);
    pixels = malloc(pixels_size);

    read = fread(pixels, 1, pixels_size, f);

    if (read != pixels_size) {
        fprintf(stderr, "%s has incomplete image\n", filename);
        fclose(f);
        free(pixels);
        return NULL;
    }

    return pixels;
}

GLuint make_texture(const char *filename)
{
    int width, height;
    void *pixels = read_tga(filename, &width, &height);
    GLuint texture;

    if (!pixels)
        return 0;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    glTexImage2D(
        GL_TEXTURE_2D, 0,           /* target, level */
        GL_RGB8,                    /* internal format */
        width, height, 0,           /* width, height, border */
        GL_BGR, GL_UNSIGNED_BYTE,   /* external format, type */
        pixels                      /* pixels */
    );
    free(pixels);
    return texture;
}


/// This part has been added by Marco Gribaudo
#define SHADER_CODE_LEN 16384
char shader_code[2][SHADER_CODE_LEN];

#define LINE_BUF 1024

void LoadCode(char *fs, int pos) {
	FILE *fp;
	static char line[LINE_BUF];
	shader_code[pos][0] = 0;
	
	fp = fopen(fs, "r");
	if(fp == NULL) {
		printf("Cannot find %s shader file: >%s<\n", (pos == 0 ? "vertex" : "fragment"), fs);
		exit(1);
	}
	
	while(!feof(fp)) {
		if(fgets(line, LINE_BUF, fp) != NULL) {
			if(strlen(shader_code[pos]) + strlen(line) < SHADER_CODE_LEN) {
				strcat(shader_code[pos], line);
			}
		}
	}
	fclose(fp);
}

GLuint CompileAndLinkShaders(const char *vertex_code, const char *fragment_code){
	GLint Result = GL_FALSE;	// for error checking
	int InfoLogLength;	
	char *errMsg;

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint ProgramID = glCreateProgram();

//printf("%s\n", vertex_code);
	// Compile Vertex Shader
	glShaderSource(VertexShaderID, 1, (const GLchar **)&vertex_code, NULL);
	glCompileShader(VertexShaderID);

	// ERROR Checking
	fprintf(stdout, "Vertex Shader:\n");
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	errMsg = (char *)malloc(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, errMsg);
	fprintf(stdout, "%s\n", errMsg);
	free(errMsg);


//printf("%s\n", fragment_code);
	// Compile Fragment Shader
	glShaderSource(FragmentShaderID, 1, (const GLchar **)&fragment_code, NULL);
	glCompileShader(FragmentShaderID);

	// ERROR Checking
	fprintf(stdout, "Fragment Shader:\n");
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	errMsg = (char *)malloc(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, errMsg);
	fprintf(stdout, "%s\n", errMsg);
	free(errMsg);


	// Compile and link the program
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// ERROR Checking
	fprintf(stdout, "Program:\n");
	glGetShaderiv(ProgramID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	errMsg = (char *)malloc(InfoLogLength);
	glGetShaderInfoLog(ProgramID, InfoLogLength, NULL, errMsg);
	fprintf(stdout, "%s\n", errMsg);
	free(errMsg);


	// free the shader, since now they are in the program
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

GLuint LoadCompileAndLinkShaders(char *vs, char *fs){
	// Compile Vertex Shader
	LoadCode(vs, 0);
	LoadCode(fs, 1);

	return CompileAndLinkShaders(&shader_code[0][0], &shader_code[1][0]);
}

