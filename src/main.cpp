#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include <chrono>
#include <iostream>


SDL_Window* window = NULL;
SDL_GLContext context;
	
const GLchar* vertexSource = "\
#version 120\n\
attribute vec2 position;\
attribute vec3 color;\
varying vec3 Color;\
void main()\
{\
	Color = color;\
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);\
}\
";

const GLchar* fragmentSource = "\
#version 120\n\
varying vec3 Color;\
void main()\
{\
    gl_FragColor = vec4(Color, 1.0);\
}\
";

bool initGL()
{
    bool success = true;
    GLenum error = GL_NO_ERROR;
    // Initialize Projection  Matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    // Check for error
    error = glGetError();
    if(error != GL_NO_ERROR) {
		printf("Error initializing OpenGL! \n");
		success = false;
    }
    // Initialize  Modelview  Matrix 
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Check for error
    error = glGetError();
    if(error != GL_NO_ERROR) {
		printf("Error initializing OpenGL! \n");
		success = false;
    }
    // Initialize clear color
    glClearColor(0.f, 0.f, 0.f, 1.f);
    // Check for error
    error = glGetError();
    if(error != GL_NO_ERROR) {
		printf("Error initializing OpenGL! \n");
		success = false;
    }
    return success;
}


// Initialize SDL and GLEW
bool initSDL(int screenWidth, int screenHeight) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
    } else {
	// Use OpenGL 2.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	
	// Create window
	window = SDL_CreateWindow("SDL Tutorial", 100, 100, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
	if(window == NULL) {
	    printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	} else {
	    // Create context
	    context = SDL_GL_CreateContext(window);
		
	    if(context == NULL) {
			printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
			return false;
	    } else {
			// Use Vsync
			if(SDL_GL_SetSwapInterval(1) < 0) {
				printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
			}
			// Initialize OpenGL
			if(!initGL()) {
				printf("Unable to initialize OpenGL!\n");
				return false;
			}
	    }
	}
	}

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
	
	
	if(err != GLEW_OK) {
		// Problem: glewInit failed, something is seriously wrong.
		std::cout << "glewInit failed, aborting. Code " << err << ". " << std::endl;
		return false;
	}
	
	return true;
}

int main()
{

	initSDL(640, 480);
	
    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLfloat vertices[] = { 
		0.0f, 0.5f, 1.0f, 0.5f, 0.0f, 
		0.5f,-0.5f, 0.0f, 0.2f, 0.6f,
		-0.5f,-0.5f, 0.7f, 0.0f, 0.5f
	};

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
	
	// debug shader compilation
	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if(status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
		std::cout << "Vertex shader did not compile: " << buffer << std::endl;
	}

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

	// debug shader compilation
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if(status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
		std::cout << "Fragment shader did not compile: " << buffer << std::endl;
	}

    // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
	
	GLint position = 0, colour = 1;
	glBindAttribLocation(shaderProgram, position , "position");
	glBindAttribLocation(shaderProgram, colour , "color");
	
    glLinkProgram(shaderProgram);
	
	int IsLinked = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int *)&IsLinked);
    if(IsLinked == GL_FALSE)
    {
       return 1;
    }
	
    glUseProgram(shaderProgram);

    // Specify the layout of the vertex data
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
	
	
    glEnableVertexAttribArray(colour);
    glVertexAttribPointer(colour, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
	
	if(glGetError() == GL_TRUE) {
			std::cout<<"Shit happens!";
	}
	
    SDL_Event windowEvent;
    while(true) {
	
	if(SDL_PollEvent(&windowEvent)) {
	    if(windowEvent.type == SDL_QUIT)
		break;
	}
	
	// Clear the screen to black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw a triangle from the 3 vertices
	glDrawArrays(GL_TRIANGLES, 0, 3);
	
	// Swap buffers
	SDL_GL_SwapWindow(window);



    }

    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteBuffers(1, &vbo);

    glDeleteVertexArrays(1, &vao);
    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}
