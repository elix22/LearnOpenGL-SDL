#ifndef _LEARNGL_UTILS_C
#define _LEARNGL_UTILS_C

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#ifdef ANDROID

#include <SDL.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define glGenVertexArrays glGenVertexArraysOES
#define glBindVertexArray glBindVertexArrayOES

#define  ASSETS_DIR ""
#else

#include <GL/glew.h>
#include <GL/glut.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>

#define  ASSETS_DIR "assets/"

#endif

char * utilLoadFile(const char * filename, int * size) {

    SDL_RWops * file;
    char * buffer;

    char filename_final[256] = "";
    strcpy(filename_final, ASSETS_DIR );
    strcat(filename_final, filename);

    file = SDL_RWFromFile(filename_final, "rb");
    if (file==NULL) {
        SDL_Log("Unable to open file");
        return 0;
    }

    SDL_RWseek(file, 0, SEEK_END);
    int finalPos = SDL_RWtell(file);
    SDL_RWclose(file);

    /* allocate memory for entire content */
    buffer = (char*)calloc( 1, sizeof(char) * (finalPos + 1));

    file = SDL_RWFromFile(filename_final, "rb");
    int n_blocks = SDL_RWread(file, buffer, 1, finalPos);
    if(n_blocks < 0) {
        SDL_Log("Unable to read any block");
    }
    if(n_blocks == 0) {
        SDL_Log("No block read");
    }

    *size = finalPos;

    SDL_RWclose(file);
    return buffer;
}

#endif
