//
// Created by XposeMedia on 18/05/2017.
//

//Android-Sample\SDL\app\src\main\jni>ndk-build -j4 NDK_LIBS_OUT=../jniLibs
// ndk-build -j4 NDK_LIBS_OUT=../jniLibs

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

#include "utils.h"

#define APPNAME "SDL"


static int WinWidth =  640;
static int WinHeight = 480;

static unsigned int WindowFlags = SDL_WINDOW_OPENGL;
static SDL_Window *Window = 0;

void 	initSDL();
void 	initGL();
GLint  	createVertextShader(const char * shaderName);
GLint 	createFragmentShader(const char * shaderName);
GLint 	createFragmentShader(const char * shaderName);
GLint 	createProgram(int vertextShaderID , int fragmentShaderID);
GLuint 	createStaticVBO(float * vertices , int vertices_size);
void 	prepareDraw(GLint programID,GLint VBO);
void 	draw();
void 	quitSDL();
int  	isRunning();


int SDL_main(int argc, char **argv)
{
	
	// NDC - Normalized Device Coordinates
    float triangleVertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 

    };
	
	
								initSDL();
			                    initGL();
	GLint vertextShaderID  = 	createVertextShader("VertexShader.glsl");
	GLint fragmentShaderID = 	createFragmentShader("FragmentShader.glsl");
	GLint programID = 			createProgram(vertextShaderID , fragmentShaderID);
	GLuint VBO = 				createStaticVBO(triangleVertices , sizeof(triangleVertices));
							
    while (isRunning())
    {
		prepareDraw(programID, VBO);
		draw();
    }
	
	glDeleteBuffers(1, &VBO);
	quitSDL();
   
    return 0;
}


void initSDL()
{
    SDL_Init(SDL_INIT_VIDEO);
	int x = 100 ,y =100;
#ifdef ANDROID
    SDL_DisplayMode DM;
   // SDL_GetDesktopDisplayMode(0, &DM);
    SDL_GetCurrentDisplayMode(0, &DM);
    WinWidth = DM.w;
    WinHeight = DM.h;
	x=0;
	y=0;
#endif

    Window = SDL_CreateWindow("OpenGL Hello Triangle", x, y, WinWidth, WinHeight, WindowFlags);
    assert(Window);
    SDL_GLContext Context = SDL_GL_CreateContext(Window);
	


}

void initGL()
{
#ifndef ANDROID
    glewInit();
#endif
	glViewport(0, 0, WinWidth, WinHeight);
}


GLint  createVertextShader(const char * shaderName)
{
	int success = 0;
    char infoLog[512]={0};
	int size=0;
	char * shaderSource = utilLoadFile(shaderName, &size);
	
	GLint vertexShaderObjectID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderObjectID, 1, &shaderSource, NULL);
    glCompileShader(vertexShaderObjectID);
	
	// check for shader compile errors
    glGetShaderiv(vertexShaderObjectID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderObjectID, 512, NULL, infoLog);
        SDL_Log("ERROR::SHADER::VERTEX::COMPILATION_FAILED %s\n",infoLog);
    }
	free(shaderSource);
	return vertexShaderObjectID;
	
}

GLint createFragmentShader(const char * shaderName)
{
	int success = 0;
    char infoLog[512]={0};
	int size=0;
	char * shaderSource = utilLoadFile(shaderName, &size);
	// fragment shader
    GLint fragmentShaderObjectID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObjectID, 1, &shaderSource, NULL);
    glCompileShader(fragmentShaderObjectID);
    // check for shader compile errors
    glGetShaderiv(fragmentShaderObjectID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderObjectID, 512, NULL, infoLog);
        SDL_Log("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED %s\n",infoLog);
    }
	free(shaderSource);
	return fragmentShaderObjectID;
}


GLint createProgram(int vertextShaderID , int fragmentShaderID)
{
	int success = 0;
    char infoLog[512]={0};
	// link shaders
    GLint shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertextShaderID);
    glAttachShader(shaderProgramID, fragmentShaderID);
    glLinkProgram(shaderProgramID);
    // check for linking errors
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
        SDL_Log("ERROR::SHADER::PROGRAM::LINKING_FAILED %s\n",infoLog);
    }
	
	glDeleteShader(vertextShaderID);
    glDeleteShader(fragmentShaderID);
	
	return shaderProgramID;
}

GLuint createStaticVBO(float * vertices , int vertices_size)
{
	GLuint VBO;
	glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
	return VBO;
}

void prepareDraw(GLint programID,GLint VBO)
{
    glUseProgram(programID);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	// position attribute
	GLint aPosLocation = glGetAttribLocation(programID,"aPos");
	glEnableVertexAttribArray(aPosLocation);
    glVertexAttribPointer(aPosLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    
    // color attribute
	GLint aColorLocation = glGetAttribLocation(programID,"aColor");
	glEnableVertexAttribArray(aColorLocation);
    glVertexAttribPointer(aColorLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}


void draw()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);
		
    SDL_GL_SwapWindow(Window);
}


void quitSDL()
{
	SDL_DestroyWindow(Window);
    SDL_Quit();
}

int  isRunning()
{
    int Running = 1;

    SDL_Event Event;
    while (SDL_PollEvent(&Event))
    {
        if (Event.type == SDL_KEYDOWN)
        {
            switch (Event.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    Running = 0;
                    break;
                default:
                    break;
            }
        }
        else if (Event.type == SDL_QUIT)
        {
            Running = 0;
        }
    }

    return Running;
}

