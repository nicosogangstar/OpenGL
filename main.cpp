// Standard
#include <stdio.h>
#include <stdlib.h>

// Misc
#include <vector>
#include <iostream>
#include <fstream>

// GL
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#define width 1024
#define height 786

static GLdouble bounds[] = {
	-2.0f, 2.0f, -2.0f, 2.0f
};

static GLdouble viewportDimensions[] {
	width, height
};

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()) {
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Unable to open %s.\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()) {
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if(InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if(InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if(InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

int zoom(GLfloat rangeModifier) {
	GLfloat rangeI = bounds[1] - bounds[0];
	GLfloat newRangeI;
	newRangeI = rangeI * rangeModifier;
	GLfloat deltaI = newRangeI - rangeI;
	bounds[0] -= deltaI / 2;
	bounds[1] = bounds[0] + newRangeI;
	
	GLfloat rangeR = bounds[3] - bounds[2];
	GLfloat newRangeR;
	newRangeR = rangeR * rangeModifier;
	GLfloat deltaR = newRangeR - rangeR;
	bounds[2] -= deltaR / 2;
	bounds[3] = bounds[2] + newRangeR;
}

int pan(GLfloat distI, GLfloat distR) {
	GLfloat rangeI = bounds[1] - bounds[0];
	GLfloat rangeR = bounds[3] - bounds[2];

	GLfloat deltaI = (distR / height) * rangeI;
	GLfloat deltaR = (distI / width) * rangeR;

	bounds[0] += deltaI;
	bounds[1] += deltaI;
	bounds[2] -= deltaR;
	bounds[3] -= deltaR;
}

int main(int argc, char** argv) {
    // Initialise GLFW
	if(!glfwInit()) {
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}
	
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL 4.4
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Don't want the old OpenGL 
	
	// Open a window and create its OpenGL context
	GLFWwindow* window; // (In the accompanying source code, this variable is global)
	window = glfwCreateWindow(width, height, "Nico's OpenGL App", NULL, NULL);
	if(window == NULL) {
	    fprintf( stderr, "Failed to open GLFW window.\n" );
	    glfwTerminate();
	    return -1;
	}
	glfwMakeContextCurrent(window); // Initialize GLEW
	
	glewExperimental = true; // Needed in core profile
	if(glewInit() != GLEW_OK) {
	    fprintf(stderr, "Failed to initialize GLEW\n");
	    return -1;
	}
	
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	// An array of vec2s, representing 2d vertices
	static const GLfloat g_vertex_buffer_data[] = {
	    -1.0f,  1.0f, // Triangle 1
		-1.0f, -1.0f,
		 1.0f, -1.0f,

		-1.0f,  1.0f, // Triangle 2
		 1.0f,  1.0f,
		 1.0f, -1.0f
	};
	
	// Vertex buffer id
	GLuint vertexbuffer;
	// Generate a buffer and put the resulting identifier in it
	glGenBuffers(1, &vertexbuffer);
	// Bind the buffer to OpenGL
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give OpenGL the verticies
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	
	
	// Load shaders
	GLuint programID = LoadShaders("main.vs.glsl", "main.fs.glsl");
	
	GLuint viewportLoc = glGetUniformLocation(programID, "viewportDimensions");
	GLuint boundsLoc = glGetUniformLocation(programID, "bounds");
	
	// Main loop
	do {
		// Draw
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Use our shader
		glUseProgram(programID);
		
		// Position attribute
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0, // attribute index
			2, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized
			0, // stride
			(void*)0 // array buffer offset
		);
		glDrawArrays(GL_TRIANGLES, 0, 3*2);
		glDisableVertexAttribArray(0);
		
		// Uniforms
		glUniform2dv(viewportLoc, 1, viewportDimensions);
		glUniform4dv(boundsLoc, 1, bounds);
		
		GLfloat zoomSpeed = 0.03f;
		GLfloat panSpeed = 3.0;
		
		//Keys
		if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
			zoom(1.0 - zoomSpeed);
		}
		if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
			zoom(1.0 + zoomSpeed);
		}
		
		if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
			pan(0, panSpeed);
		}
		if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
			pan(panSpeed, 0);
		}
		if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
			pan(0, -panSpeed);
		}
		if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
			pan(-panSpeed, 0);
		}
		
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
}
