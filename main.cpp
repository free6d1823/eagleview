// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"
#include "basicobject.h"
#include "sphere.h"
#include "car.h"

bool	g_bShowCar = true;
Car   g_car;

void onKeyEvent(GLFWwindow* hWindow, int key, int scancode, int action, int mods);
int main(int argc, char **argv) {
	GLFWwindow* hWindow;

	Sphere	m_background;


	printf("Hello All !\n");

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	hWindow = glfwCreateWindow( 1024, 768, "Tutorial 09 - Rendering several models", NULL, NULL);
	if( hWindow == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(hWindow);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(hWindow, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(hWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(hWindow, 1024/2, 768/2);

    //set key callback
    glfwSetInputMode(hWindow, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetKeyCallback(hWindow, onKeyEvent);
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.3f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
//x	glEnable(GL_CULL_FACE);
	m_background.init();
	g_car.init();
	double lastTime = glfwGetTime();
	int nbFrames = 0;

	do{
		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
			// printf and reset
//x			printf("%f ms/frame\n", 1000.0/double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs2(hWindow);
		/////
		mat4 ProjectionMatrix = getProjectionMatrix2();
		mat4 ViewMatrix = getViewMatrix2();
		vec3 lightPos = glm::vec3(4,8,4);
        mat4 objectMatrix = getObjectMoveMat();
        g_car.transform(objectMatrix);

        ////// Start of the rendering of the first object //////
        m_background.update(ProjectionMatrix, ViewMatrix, lightPos);
        m_background.draw();
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		g_car.update(ProjectionMatrix, ViewMatrix, lightPos);
		if(g_bShowCar)
			g_car.draw();

		// Swap buffers
		glfwSwapBuffers(hWindow);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(hWindow, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
			glfwWindowShouldClose(hWindow) == 0 );
	//clean up objects
	g_car.cleanup();
	m_background.cleanup();
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return 0;
}
