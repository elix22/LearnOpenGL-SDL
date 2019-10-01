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

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

#define HANDMADE_MATH_IMPLEMENTATION
#include "HandmadeMath.h"

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

GLuint createTexture(const char * imageFilePath,GLenum type)
{
	GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_Surface* sdlSurface =  STBIMG_Load(imageFilePath);
	
	if(sdlSurface != NULL) 
	{
		glTexImage2D(GL_TEXTURE_2D, 0, type, sdlSurface->w, sdlSurface->h, 0, type, GL_UNSIGNED_BYTE, sdlSurface->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
		SDL_FreeSurface(sdlSurface);
	}
	else
	{
      SDL_Log("ERROR: Couldn't load %s, reason: %s\n", imageFilePath, SDL_GetError());
    }
	

	return textureID;
	
}

GLuint createTextureRGB(const char * imageFilePath)
{
	return createTexture(imageFilePath,GL_RGB);
}

GLuint createTextureRGBA(const char * imageFilePath)
{
	return createTexture(imageFilePath,GL_RGBA);
}


void prepareDraw(GLint programID,GLint VBO,GLuint textureID1,GLuint textureID2)
{
	
    glUseProgram(programID);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureID2);
	
	GLint attributePosLocation = glGetAttribLocation(programID,"aPos");
	glVertexAttribPointer(attributePosLocation, 3 , GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(attributePosLocation);
	
	GLint attributeTextureLocation = glGetAttribLocation(programID,"aTexCoord");
	glVertexAttribPointer(attributeTextureLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(attributeTextureLocation);
	
	glUniform1i(glGetUniformLocation(programID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(programID, "texture2"), 1);
	

	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}


void draw(GLint programID)
{
	static float counter =0;
		
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

	    hmm_vec3 cubePositions[] = {
        HMM_Vec3( 0.0f,  0.0f,  0.0f),
        HMM_Vec3( 2.0f,  5.0f, -15.0f),
        HMM_Vec3(-1.5f, -2.2f, -2.5f),
        HMM_Vec3(-3.8f, -2.0f, -12.3f),
        HMM_Vec3( 2.4f, -0.4f, -3.5f),
        HMM_Vec3(-1.7f,  3.0f, -7.5f),
        HMM_Vec3( 1.3f, -2.0f, -2.5f),
        HMM_Vec3( 1.5f,  2.0f, -2.5f),
        HMM_Vec3( 1.5f,  0.2f, -1.5f),
        HMM_Vec3(-1.3f,  1.0f, -1.5f)
    };
	
	// create transformations
	hmm_mat4 view =  HMM_Translate(HMM_Vec3(0.0f, 0.0f, -7.0f));
	
	hmm_mat4 projection = HMM_Perspective(45.0f, (float)WinWidth/WinHeight, 0.1f, 100.0f);
	
    GLuint modelLoc = glGetUniformLocation(programID, "model");
    GLuint  viewLoc  = glGetUniformLocation(programID, "view");
	GLuint  projectionLoc  = glGetUniformLocation(programID, "projection");
	
	
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (const GLfloat*)view.Elements);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (const GLfloat*)projection.Elements);
	
	float rotationSign = 1.0f;
	for (unsigned int i = 0; i < 10; i++)
    {
		    
            // calculate the model matrix for each object and pass it to shader before drawing
			hmm_mat4 model =  HMM_Translate(cubePositions[i]);
			hmm_mat4 model_rotate = HMM_Rotate(rotationSign*(counter+i*10), HMM_Vec3(0.5f, 1.0f, 0.0f));
			model = HMM_MultiplyMat4(model, model_rotate);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (const GLfloat*)model.Elements);

            glDrawArrays(GL_TRIANGLES, 0, 36);
			rotationSign =  - rotationSign;
    }
		
		
    SDL_GL_SwapWindow(Window);
	
	counter +=0.5f;
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
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
	
 
	
								initSDL();
			                    initGL();
	GLint vertextShaderID  = 	createVertexShader("VertexShader.glsl", NULL);
	GLint fragmentShaderID = 	createFragmentShader("FragmentShader.glsl", "#define BLUE_COLOR");
	GLint programID = 			createProgram(vertextShaderID , fragmentShaderID);
	GLuint VBO = 				createStaticVBO(vertices , sizeof(vertices));
	GLuint textureID1 =  createTextureRGB("container.jpg");
	GLuint textureID2 =  createTextureRGBA("awesomeface.png");

	glEnable(GL_DEPTH_TEST);
	
    while (isRunning())
    {
		prepareDraw(programID, VBO,textureID1,textureID2);
		draw(programID);
    }
	
	glDeleteBuffers(1, &VBO);
	glDeleteTextures(1,&textureID1);
	glDeleteTextures(1,&textureID2);
	
	quitSDL();
   
    return 0;
}