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

    Window = SDL_CreateWindow("2-GLSL-Defines", x, y, WinWidth, WinHeight, WindowFlags);
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

GLint  createShader(const char * shaderName,GLenum shaderType, const char * defines)
{
	int success = 0;
    char infoLog[512]={0};
	int size=0;
	char * shaderSource = utilLoadFile(shaderName, &size);
	
	if(defines != NULL)
	{
		char * shaderSourceWithDefines = (char*)calloc( 1, sizeof(char) * (size + 1 + strlen(defines) + 1));
		strcpy(shaderSourceWithDefines,defines);
		strcat(shaderSourceWithDefines,shaderSource);
		free(shaderSource);
		shaderSource = shaderSourceWithDefines;
	}
	
	GLint shaderObjectID = glCreateShader(shaderType);
    glShaderSource(shaderObjectID, 1, &shaderSource, NULL);
    glCompileShader(shaderObjectID);
	
	// check for shader compile errors
    glGetShaderiv(shaderObjectID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderObjectID, 512, NULL, infoLog);
        SDL_Log("ERROR::SHADER::VERTEX::COMPILATION_FAILED %s\n",infoLog);
    }
	
	free(shaderSource);
	return shaderObjectID;
}

GLint  createVertexShader(const char * shaderName, const char * defines)
{
	return createShader(shaderName,GL_VERTEX_SHADER, defines);
}

GLint createFragmentShader(const char * shaderName, const char * defines)
{
	return createShader(shaderName,GL_FRAGMENT_SHADER, defines);
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
	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return VBO;
}

GLuint createStaticEBO(unsigned int  * indices , int indices_size)
{
	GLuint EBO;
	glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return EBO;
}

void prepareDraw(GLint programID,GLint VBO,GLint EBO)
{
	// draw our first triangle
    glUseProgram(programID);
	GLint attributeLocation = glGetAttribLocation(programID,"aPos");
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(attributeLocation, 3 , GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(attributeLocation);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}


void draw()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
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

int SDL_main(int argc, char **argv)
{
	
	// NDC - Normalized Device Coordinates
      float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
	
	unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
	
								initSDL();
			                    initGL();
	GLint vertextShaderID  = 	createVertexShader("VertexShader.glsl", NULL);
	GLint fragmentShaderID = 	createFragmentShader("FragmentShader.glsl", "#define BLUE_COLOR");
	GLint programID = 			createProgram(vertextShaderID , fragmentShaderID);
	GLuint VBO = 				createStaticVBO(vertices , sizeof(vertices));
	GLuint EBO = 				createStaticEBO(indices , sizeof(indices));						
    while (isRunning())
    {
		prepareDraw(programID, VBO,EBO);
		draw();
    }
	
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	
	quitSDL();
   
    return 0;
}