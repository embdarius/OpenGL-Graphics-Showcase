//
//  main.cpp
//  OpenGL Transformations
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/glm.hpp>//core glm functionality
#include <glm/gtc/matrix_transform.hpp>//glm extension for generating common transformation matrices
#include <glm/gtc/type_ptr.hpp>//glm extension for accessing the internal data structure of glm types
#include <glm/gtc/matrix_inverse.hpp>

#include "Shader.hpp"
#include "Camera.hpp"

int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

//vertex data --- position and color
//vertex position data has been duplicated to ensure constant color across each face
GLfloat vertices[] = {
	-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, //0
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, //1
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, //2
	-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, //3
	-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, //4
	0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, //5
	0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, //6
	-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, //7
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, //8   0'
	0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, //9   1'
	0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, //10   2'
	-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, //11   3'
	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, //12   4'
	0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, //13   5'
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, //14   6'
	-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, //15   7'
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, //16   0''
	0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, //17   1''
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, //18   2''
	-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, //19   3''
	-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, //20   4''
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, //21   5''
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, //22   6''
	-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, //23   7''
};

GLuint indices[] = {
	0, 1, 2, 0, 2, 3, // bottom plane triangles
	8, 9, 5, 8, 5, 4, // back plane triangles
	17, 10, 6, 17, 6, 13, // right plane triangles
	16, 12, 7, 16, 7, 11, // left plane triangles
	20, 21, 14, 20, 14, 15, // top plane triangles
	19, 18, 22, 19, 22, 23 // front plane triangles
};

GLuint verticesVBO;
GLuint verticesEBO;
GLuint objectVAO;

gps::Shader cubeShader;


glm::mat4 model;
GLint modelLocation;

glm::mat4 view;
GLint viewLocation;

glm::mat4 projection;
GLint projectionLocation;

gps::Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));


bool wireframeView = false;


void windowResizeCallback(GLFWwindow* window, int width, int height) {

	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {

		glfwSetWindowShouldClose(glWindow, GLFW_TRUE);
	}

	if (key == GLFW_KEY_T && action == GLFW_PRESS) {

		wireframeView = !wireframeView;

		if (wireframeView) {

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else {

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
}

// Mouse-related variables
float lastX = glWindowWidth / 2.0f; // Initialize to the center of the screen
float lastY = glWindowHeight / 2.0f;
bool firstMouse = true;
float yaw = -90.0f; // Initial yaw (facing forward)
float pitch = 0.0f; // Initial pitch
float sensitivity = 0.1f; // Mouse sensitivity

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos; // Reversed: y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	camera.rotate(pitch, yaw);
}

void initObjects() {

	glGenVertexArrays(1, &objectVAO);
	glBindVertexArray(objectVAO);

	glGenBuffers(1, &verticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &verticesEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, verticesEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//vertex position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//vertex colour attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

bool initOpenGLWindow() {

	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	// window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//window scaling for HiDPI displays
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	// for multisampling/antialising
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Transformations", NULL, NULL);

	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");		
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, key_callback);

	glfwSetCursorPosCallback(glWindow, mouse_callback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);	


	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}


float angle = 0.0f;
float movement_speed = 5.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
void checkForMovement() {
	if (glfwGetKey(glWindow, GLFW_KEY_A) == GLFW_PRESS) {
		camera.move(gps::MOVE_LEFT, movement_speed * deltaTime);
	}
	if (glfwGetKey(glWindow, GLFW_KEY_D) == GLFW_PRESS) {
		camera.move(gps::MOVE_RIGHT, movement_speed * deltaTime);
	}
	if (glfwGetKey(glWindow, GLFW_KEY_W) == GLFW_PRESS) {
		camera.move(gps::MOVE_FORWARD, movement_speed * deltaTime);
	}
	if (glfwGetKey(glWindow, GLFW_KEY_S) == GLFW_PRESS) {
		camera.move(gps::MOVE_BACKWARD, movement_speed * deltaTime);
	}
}

void renderScene() {

	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, retina_width, retina_height);
	checkForMovement();

	// update view matrix ( for camera movement )
	view = camera.getViewMatrix();
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

	// DRAW CUBE 
	cubeShader.useShaderProgram();
	glBindVertexArray(objectVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void cleanup() {

	if (verticesVBO) {
		glDeleteBuffers(1, &verticesVBO);
	}

	if (verticesEBO) {
		glDeleteBuffers(1, &verticesEBO);
	}

	if (objectVAO) {
		glDeleteVertexArrays(1, &objectVAO);
	}

	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

void initModelViewProjection() {
	model = glm::mat4(1.0f);
	modelLocation = glGetUniformLocation(cubeShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

	view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	viewLocation = glGetUniformLocation(cubeShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

	float aspect_ratio = (float)glWindowWidth / (float)glWindowHeight;
	projection = glm::perspective(70.0f, aspect_ratio, 0.1f, 1000.0f);
	projectionLocation = glGetUniformLocation(cubeShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
}

void calculateNewDelta() {
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}


glm::vec3 lightColor;
GLuint lightColorLoc;

void initLightSources() {
	// light color for cube ambient ( cubeShader )
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	lightColorLoc = glGetUniformLocation(cubeShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

}



int main(int argc, const char * argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initObjects();

	glEnable(GL_DEPTH_TEST);

	cubeShader.loadShader("shaders/cubeShader.vert", "shaders/cubeShader.frag");
	cubeShader.useShaderProgram();

	initModelViewProjection();
	initLightSources();

	while (!glfwWindowShouldClose(glWindow)) {
		// calculate delta before rendering scene
		//for smooth motion with W A S D
		calculateNewDelta();
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
