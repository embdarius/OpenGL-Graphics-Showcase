

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>
#include <Windows.h>



// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

glm::mat4 cottageModel;
glm::mat3 cottageNormalMatrix;


glm::mat4 airplaneModel;
glm::mat3 airplaneNormalMatrix;

glm::mat4 newObjectModel;
glm::mat4 groundModel;
glm::mat4 groundNormalMatrix;

glm::mat4 snowModel;
glm::mat4 snowNormalMatrix;


glm::mat4 carModel;
glm::mat4 carNormalMatrix;

glm::mat4 winterHouseModel;
glm::mat4 winterHouseNormalMatrix;

glm::mat4 lightSourceModel;
glm::mat4 lightSourceModel2;

glm::mat4 lightRotation;

glm::mat4 groundObjModel;
glm::mat4 groundObjNormalMatrix;

glm::mat4 lightCubeModel;

glm::mat4 snowmanModel;
glm::mat4 snowmanNormalMatrix;

glm::mat4 shovelModel;
glm::mat4 shovelNormalMatrix;

struct RainModel {
    glm::vec3 position;
    glm::mat4 modelMatrix;
};



// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;
GLint thunderIntensityLoc;
GLint fogEnableLoc;

// Point light 
glm::vec3 pointLightPosition;
glm::vec3 pointLightColor;
float pointLightIntensity;
float pointLightRadius; 
bool thunderEnabled;
float thunderInterval = 3.5f;
bool fallShovel = false; 
bool enableWeather = true;
float groundY = 0;
GLfloat groundVertices[] = {
    // positions          // colors            // texture coords
    -3.0f,  0.0f, -3.0f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,  // Bottom-left corner
     3.0f,  0.0f, -3.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,  // Bottom-right corner
     3.0f,  0.0f,  3.0f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,  // Top-right corner
    -3.0f,  0.0f,  3.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f   // Top-left corner
};

GLfloat snowVertices[] = {
    // positions   //texture coords
    -3.0f,  0.0f, -3.0f,  0.0f, 0.0f,  //Bottom-left corner
     3.0f,  0.0f, -3.0f,  1.0f, 0.0f,  //Bottom-right corner
     3.0f,  0.0f,  3.0f,  1.0f, 1.0f,  //Top-right corner
    -3.0f,  0.0f,  3.0f,  0.0f, 1.0f   //Top-left corner
}; 

GLuint snowIndices[] = {
    0,1,2,
    0,2,3
};

GLuint groundIndices[] = {
    0, 1, 2,  // First triangle
    0, 2, 3   // Second triangle
};

GLuint groundVAO, groundVBO, groundEBO;
GLuint groundTexture;
GLuint snowTexture; 

GLuint snowVAO, snowVBO, snowEBO;

GLfloat lightAngle; 

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 0.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.05f;
double lastX = 1024.0f / 2.0f;
double lastY = 768.0f / 2.0f;
bool firstMouse = true;
float yaw = -90.0f; // Initial yaw (facing forward)
float pitch = 0.0f; // Initial pitch
float sensitivity = 0.1f; //Mouse sensitivity



GLboolean pressedKeys[1024];

// models
gps::Model3D teapot;
GLfloat angle;

gps::Model3D cottage;
gps::Model3D light_bulb;
gps::Model3D winter_cottage;
gps::Model3D airplane;
gps::Model3D newObject;
gps::Model3D car;
gps::Model3D winterHouse;
std::vector<glm::mat4> newObjectsCreated;
std::vector<RainModel> rainModels;

gps::Model3D screenQuad;
gps::Model3D lightCube; 
gps::Model3D groundObj;
gps::Model3D snowman;
gps::Model3D shovel;
gps::Model3D rain;

// shaders
gps::Shader myBasicShader;
gps::Shader groundShader;
gps::Shader depthMapShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader snowShader;

GLint groundModelLoc;
GLint groundViewLoc;
GLint groundProjectionLoc;

GLint snowModelLoc;
GLint snowViewLoc;
GLint snowProjectionLoc;

int retina_width, retina_height;
float lastThunderUpdateTime = 0.0f;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap = false; 

bool isPresentationCamera = false;
float angleY = 0.0f;

float currentAirplaneX = -23.0f;
GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
}


#include <vector>

float cameraProgress = 0.0f;       // Progress along the path (0.0 to 1.0)
std::vector<glm::vec3> cameraPath = {
    glm::vec3(0.0f, 5.0f, 12.0f), 
    glm::vec3(10.0f, 6.0f, 10.0f),
    glm::vec3(8.0f, 4.0f, -5.0f),
    glm::vec3(-5.0f, 5.0f, -10.0f),
    glm::vec3(0.0f, 5.0f, 12.0f)   
};

std::vector<glm::vec3> cameraPath1 = {
    glm::vec3(10.0f, 5.0f, 0.0f),   
    glm::vec3(0.0f, 5.0f, 2.0f),     
    glm::vec3(3.0f, 5.0f, 2.0f),   
    glm::vec3(3.0f, 10.0f, 2.0f),  
    glm::vec3(0.0f, 5.0f, -7.0f),  
    glm::vec3(0.0f, 5.0f, -10.0f), 
    glm::vec3(7.0f, 5.0f, -7.0f),   
    glm::vec3(10.0f, 5.0f, 0.0f)    
};

std::vector<glm::vec3> cameraPath2 = {
    glm::vec3(2.0f, 2.0f, 0.0f),    
    glm::vec3(2.0f, 10.0f, 0.0f),   
    glm::vec3(5.0f, 5.5f, 0.0f),    
    glm::vec3(5.0f, 5.5f, 8.0f), 
    glm::vec3(10.0f, 5.5f, -12.0f),   
    glm::vec3(10.0f, 5.0f, -3.0f), 
    glm::vec3(2.6f, 1.0f, 1.8f),   
    glm::vec3(6.0f, 6.0f, 3.0f),  
    glm::vec3(2.0f, 2.0f, 0.0f)     
};



float thunderDuration = 0.0f;  //Duration of the current thunder event
float thunderEventStartTime = 0.0f;  //When the current thunder event started
float thunderCooldown = 0.0f;  //Time between thunder events
float thunderIntensity = 10.0f;

int thunderFlashesRemaining = 0;  //Number of thunder flashes remaining in the current event
float timeBetweenFlashes = 0.0f;  //Time between consecutive thunder flashes
float lastFlashTime = 0.0f;  //When the last thunder flash occurred
void updateThunderIntensity(GLuint shaderProgram) {
    if (thunderEnabled) {
        float currentTime = glfwGetTime();  
        float timeElapsed = currentTime - lastThunderUpdateTime;


        if (timeElapsed >= thunderCooldown) {
           
            thunderFlashesRemaining = rand() % 2 + 2;  //Random  2 / 3 flashes

            timeBetweenFlashes = 0.2f + (rand() % 3) * 0.1f;

            thunderDuration = (rand() % 2 + 1) + 0.5f;  //  1.5s / 3s

            thunderCooldown = (rand() % 4 + 3);  // Random 3 / 6 seconds

            // Start the thunder event
            thunderEventStartTime = currentTime;
            thunderIntensity = 10.0f; 

            lastThunderUpdateTime = currentTime; 
            lastFlashTime = currentTime;  
        }

        // If thunder flashes are still ongoing, apply thunder intensity
        if (currentTime - thunderEventStartTime <= thunderDuration && thunderFlashesRemaining > 0) {
            if (currentTime - lastFlashTime >= timeBetweenFlashes) {
                glUniform1f(thunderIntensityLoc, thunderIntensity);  
                PlaySound(TEXT("sounds/thunder.wav"), NULL, SND_FILENAME | SND_ASYNC);
                lastFlashTime = currentTime; 
                thunderFlashesRemaining--;  
            }
        }

        
        if (thunderFlashesRemaining <= 0 || currentTime - thunderEventStartTime > thunderDuration) {
            glUniform1f(thunderIntensityLoc, 1.0f);  
        }
    }
    else {
        glUniform1f(thunderIntensityLoc, 1.0f); 
    }
}

void updatePresentationCamera(gps::Camera& camera, float deltaTime, int choose) {
    std::vector<glm::vec3> cameraPathChosen = cameraPath;
    if (choose == 0) {
        cameraPathChosen = cameraPath;
    }
    else if (choose == 1) {
       cameraPathChosen = cameraPath1;
    }
    else if (choose == 2) {
        cameraPathChosen = cameraPath2;
    }
    

    if (isPresentationCamera && cameraPathChosen.size() > 1) {
        cameraProgress += cameraSpeed * deltaTime;

        if (cameraProgress > 1.0f) {
            cameraProgress -= 1.0f;
        }

        
        size_t numPoints = cameraPathChosen.size();
        size_t segment = static_cast<size_t>(cameraProgress * (numPoints - 1));
        float segmentProgress = (cameraProgress * (numPoints - 1)) - segment;

        //Get the current and next points
        glm::vec3 startPoint = cameraPathChosen[segment];
        glm::vec3 endPoint = cameraPathChosen[(segment + 1) % numPoints];

        //Interpolate between the two points
        glm::vec3 interpolatedPosition = glm::mix(startPoint, endPoint, segmentProgress);

        //Update camera position and direction
        camera.cameraPosition = interpolatedPosition;
        glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);
        camera.cameraFrontDirection = glm::normalize(cameraTarget - camera.cameraPosition);

        //Update view and uniform matrices
        myBasicShader.useShaderProgram();
        view = myCamera.getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        cottageNormalMatrix = glm::mat3(glm::inverseTranspose(view * cottageModel));
        airplaneNormalMatrix = glm::mat3(glm::inverseTranspose(view * airplaneModel));
        carNormalMatrix = glm::mat3(glm::inverseTranspose(view * carModel));
        winterHouseNormalMatrix = glm::mat3(glm::inverseTranspose(view * winterHouseModel));
        groundObjNormalMatrix = glm::mat3(glm::inverseTranspose(view * groundObjModel));

        groundShader.useShaderProgram();
        glUniformMatrix4fv(groundViewLoc, 1, GL_FALSE, glm::value_ptr(view));

        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(snowViewLoc, 1, GL_FALSE, glm::value_ptr(view));
    }
}





float light1X = 0.0f;
float light1Y = 1.0f;
float light1Z = 0.0f;

bool rainOrSnow = false;
bool fogEnabled = true;
int cameraChosen = 0;

void generateNewObject(const gps::Camera& myCamera);
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (isPresentationCamera && key != GLFW_KEY_ESCAPE && key != GLFW_KEY_P) return;


    std::cout << carModel[3][1] << " ";
    glm::mat4 viewMatrix = myCamera.getViewMatrix();
    glm::vec3 cameraPos = glm::vec3(glm::inverse(viewMatrix)[3]); // Extract position from the inverse matrix
    //std::cout << snowModel[3][0] << " " << snowModel[3][1] << " " << snowModel[3][2] << std::endl;
    std::cout << "Camera Position: X = " << cameraPos.x
        << ", Y = " << cameraPos.y
        << ", Z = " << cameraPos.z << std::endl;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
    if (key == GLFW_KEY_UP) {
        std::cout << "UP";
        light1Y += 0.1;
        lightDir = glm::vec3(light1X, light1Y, light1Z);
        myBasicShader.useShaderProgram();
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
        lightSourceModel = glm::translate(glm::mat4(1.0f), lightDir);
        lightSourceModel = glm::scale(lightSourceModel, glm::vec3(0.3f, 0.3f, 0.3f));
    }
    if (key == GLFW_KEY_DOWN) {
        light1Y -= 0.1;
        lightDir = glm::vec3(light1X, light1Y, light1Z);
        myBasicShader.useShaderProgram();
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
        lightSourceModel = glm::translate(glm::mat4(1.0f), lightDir);
        lightSourceModel = glm::scale(lightSourceModel, glm::vec3(0.3f, 0.3f, 0.3f));
    }
    if (key == GLFW_KEY_RIGHT) {
        light1X += 0.1;
        lightDir = glm::vec3(light1X, light1Y, light1Z);
        myBasicShader.useShaderProgram();
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
        lightSourceModel = glm::translate(glm::mat4(1.0f), lightDir);
        lightSourceModel = glm::scale(lightSourceModel, glm::vec3(0.3f, 0.3f, 0.3f));
    }
    if (key == GLFW_KEY_LEFT) {
        light1X -= 0.1;
        lightDir = glm::vec3(light1X, light1Y, light1Z);
        myBasicShader.useShaderProgram();
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
        lightSourceModel = glm::translate(glm::mat4(1.0f), lightDir);
        lightSourceModel = glm::scale(lightSourceModel, glm::vec3(0.3f, 0.3f, 0.3f));
    }
    if (key == GLFW_KEY_Z) {
        light1Z += 0.1;
        lightDir = glm::vec3(light1X, light1Y, light1Z);
        myBasicShader.useShaderProgram();
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
        lightSourceModel = glm::translate(glm::mat4(1.0f), lightDir);
        lightSourceModel = glm::scale(lightSourceModel, glm::vec3(0.3f, 0.3f, 0.3f));
    }
    if (key == GLFW_KEY_X) {
        light1Z -= 0.1;
        lightDir = glm::vec3(light1X, light1Y, light1Z);
        myBasicShader.useShaderProgram();
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
        lightSourceModel = glm::translate(glm::mat4(1.0f), lightDir);
        lightSourceModel = glm::scale(lightSourceModel, glm::vec3(0.3f, 0.3f, 0.3f));
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        generateNewObject(myCamera);
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        isPresentationCamera = !isPresentationCamera;
        //cameraAngle = 0.0f;
    }
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        showDepthMap = !showDepthMap;
    }
    if (key == GLFW_KEY_J && action == GLFW_PRESS) {
        lightAngle -= 25.0f;
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        lightAngle += 25.0f;
    }   
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        // Set the polygon mode to points
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glPointSize(10.0f); 
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        thunderEnabled = !thunderEnabled;
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
        fallShovel = !fallShovel;
    }
    if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
        rainOrSnow = !rainOrSnow;
    }
    if (key == GLFW_KEY_6 && action == GLFW_PRESS) {
        myBasicShader.useShaderProgram();
        fogEnabled = !fogEnabled;

        if (fogEnabled) {
            glUniform1i(fogEnableLoc, 1);
        }
        else {
            glUniform1i(fogEnableLoc ,0);
        }
        
        std::cout << "FOG ENABLED: " << fogEnabled << std::endl;
    }
    if (key == GLFW_KEY_7 && action == GLFW_PRESS) {
        enableWeather = !enableWeather;
    }
    if (key == GLFW_KEY_8 && action == GLFW_PRESS) {
        cameraChosen = 0;
    }
    if (key == GLFW_KEY_9 && action == GLFW_PRESS) {
        cameraChosen = 1;
    }
    if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
        cameraChosen = 2;
    }
}



void mouseCallback(GLFWwindow* window, double xpos, double ypos) {

  
    if (isPresentationCamera) return;
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xOffset = xpos - lastX;  
    double yOffset = lastY - ypos;

    lastX = xpos; 
    lastY = ypos;

    const float sensitivity = 0.1f; //Sensitivity of the mouse movement
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    
    myCamera.rotate(pitch, yaw);

    myBasicShader.useShaderProgram();
    //update view ( camera ) 
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    cottageNormalMatrix = glm::mat3(glm::inverseTranspose(view * cottageModel)); // For cottage
    airplaneNormalMatrix = glm::mat3(glm::inverseTranspose(view * airplaneModel));
    carNormalMatrix = glm::mat3(glm::inverseTranspose(view * carModel));
    //winterHouseNormalMatrix = glm::mat3(glm::inverseTranspose(view * winterHouseModel));

    groundShader.useShaderProgram();
    glUniformMatrix4fv(groundViewLoc, 1, GL_FALSE, glm::value_ptr(view));


}


void processMovement() {


	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		//update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        groundShader.useShaderProgram();
        glUniformMatrix4fv(groundViewLoc, 1, GL_FALSE, glm::value_ptr(view));
        snowShader.useShaderProgram();
        glUniformMatrix4fv(snowViewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
        cottageNormalMatrix = glm::mat3(glm::inverseTranspose(view * cottageModel));
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        groundShader.useShaderProgram();
        glUniformMatrix4fv(groundViewLoc, 1, GL_FALSE, glm::value_ptr(view));
        snowShader.useShaderProgram();
        glUniformMatrix4fv(snowViewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
        cottageNormalMatrix = glm::mat3(glm::inverseTranspose(view * cottageModel));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        groundShader.useShaderProgram();
        glUniformMatrix4fv(groundViewLoc, 1, GL_FALSE, glm::value_ptr(view));
        snowShader.useShaderProgram();
        glUniformMatrix4fv(snowViewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
        cottageNormalMatrix = glm::mat3(glm::inverseTranspose(view * cottageModel));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        groundShader.useShaderProgram();
        glUniformMatrix4fv(groundViewLoc, 1, GL_FALSE, glm::value_ptr(view));
        snowShader.useShaderProgram();
        glUniformMatrix4fv(snowViewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
        cottageNormalMatrix = glm::mat3(glm::inverseTranspose(view * cottageModel));
	}

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));

        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    }
}

void initOpenGLWindow() {
    myWindow.Create(1024, 768, "OpenGL Project Core");


}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    //cottage.LoadModel("models/cottage/cottage_obj2.obj");
    light_bulb.LoadModel("models/light_bulb/bulb.obj");
    airplane.LoadModel("models/airplane/11803_Airplane_v1_l1.obj");
    newObject.LoadModel("models/teapot/teapot20segUT.obj"); // placeholder 
    winterHouse.LoadModel("models/winter_house/winter_house4.obj");
    lightCube.LoadModel("models/cube/cube.obj");
    screenQuad.LoadModel("models/quad/quad.obj");
    groundObj.LoadModel("models/ground/ground.obj");
    snowman.LoadModel("models/snowman/snowman.obj");
    shovel.LoadModel("models/snowman/shovel.obj");
    rain.LoadModel("models/rain/drops.obj");
}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
    myBasicShader.useShaderProgram();

    groundShader.loadShader(
        "shaders/groundShader.vert",
        "shaders/groundShader.frag"
    );
    groundShader.useShaderProgram();

    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
    lightShader.useShaderProgram();

    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
    screenQuadShader.useShaderProgram();

    depthMapShader.loadShader("shaders/shadowMap.vert", "shaders/shadowMap.frag");
    depthMapShader.useShaderProgram();

    snowShader.loadShader("shaders/snowShader.vert", "shaders/snowShader.frag");
    snowShader.useShaderProgram();
}


float airplaneScale = 0.001f;
void animateAirplane(float& currentX, float startY, float startZ, float endX, float deltaTime) {
    if (currentX > endX + 30.0f) return;

    float speed = 1.0f; 

    
    if (currentX < endX + 30.0f) {
        currentX += speed * deltaTime;  
    }
    if (airplaneScale >= 0 && currentX > 7) {
        airplaneScale -= 0.000001;
    }

   
    airplaneModel = glm::mat4(1.0f); 
    airplaneModel = glm::translate(airplaneModel, glm::vec3(currentX, startY, startZ)); 
    airplaneModel = glm::rotate(airplaneModel, 30.0f, glm::vec3(1, 0, 0));
    airplaneModel = glm::scale(airplaneModel, glm::vec3(airplaneScale));
}

void generateNewObject(const gps::Camera& myCamera) {
    
    glm::vec3 cameraPosition = myCamera.cameraPosition;
    glm::vec3 cameraFrontDirection = myCamera.cameraFrontDirection;

    
    float spawnDistance = 2.0f; 
    glm::vec3 spawnPosition = cameraPosition + spawnDistance * cameraFrontDirection;

    
    newObjectModel = glm::mat4(1.0f);
    newObjectModel = glm::translate(newObjectModel, spawnPosition);
    newObjectModel = glm::scale(newObjectModel, glm::vec3(0.5f)); 

    
    newObjectsCreated.push_back(newObjectModel);
}

void generateRain() {
    
    std::srand(static_cast<unsigned int>(std::time(0)));

   
    rainModels.clear();

    //Generate 1000 random rain models
    for (int i = 0; i < 2000; ++i) {
        float x = static_cast<float>(rand() % 12); 
        float y = static_cast<float>(rand() % 12); 
        float z = static_cast<float>(rand() % 12); 

       
        RainModel rainModel;
        rainModel.position = glm::vec3(x, y, z);

        
        rainModel.modelMatrix = glm::translate(glm::mat4(1.0f), rainModel.position);
        rainModel.modelMatrix = glm::scale(rainModel.modelMatrix, glm::vec3(0.05f));

        
        rainModels.push_back(rainModel);
    }
}

void initSnowflake() {
    myBasicShader.useShaderProgram();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
    glGenVertexArrays(1, &snowVAO);
    glBindVertexArray(snowVAO);

    
    glGenBuffers(1, &snowVBO);
    glBindBuffer(GL_ARRAY_BUFFER, snowVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(snowVertices), snowVertices, GL_STATIC_DRAW);

    
    glGenBuffers(1, &snowEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, snowEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(snowIndices), snowIndices, GL_STATIC_DRAW);

    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);  //Position (x, y, z)
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));  //Texture coordinates (u, v)
    glEnableVertexAttribArray(1);

    
    glBindVertexArray(0);

    //Load the texture
    glGenTextures(1, &snowTexture);
    glBindTexture(GL_TEXTURE_2D, snowTexture);
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "snowTexture"), 0);

    //Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  

    //Load the image
    int width, height, nrChannels;
    unsigned char* data = stbi_load("textures/snowflake.png", &width, &height, &nrChannels, 0);

    if (data) {
       
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);  

        stbi_image_free(data);  
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
}

void initGround() {

    groundShader.useShaderProgram();
   
    glGenVertexArrays(1, &groundVAO);
    glBindVertexArray(groundVAO);

    
    glGenBuffers(1, &groundVBO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);

   
    glGenBuffers(1, &groundEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);  //Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));  //Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));  //Texture coordinates
    glEnableVertexAttribArray(2);

    
    glBindVertexArray(0);

    
    glGenTextures(1, &groundTexture);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
    glUniform1i(glGetUniformLocation(groundShader.shaderProgram, "groundTexture"), 0);


    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

    //Load the image
    int width, height, nrChannels;
    unsigned char* data = stbi_load("textures/water.png", &width, &height, &nrChannels, 0);

    if (data) {
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);  

        stbi_image_free(data);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
}




void initFBO() {
    glGenFramebuffers(1, &shadowMapFBO);
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    //attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,
        0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initUniforms() {
	myBasicShader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    // Cottage model initialization
    cottageModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); // Adjust position
    cottageModel = glm::scale(cottageModel, glm::vec3(0.03f));
    cottageNormalMatrix = glm::mat3(glm::inverseTranspose(view * cottageModel));


    //Airplane model init
    airplaneModel = glm::translate(glm::mat4(1.0f), glm::vec3(-23.0f, 9.0f, -21.0f));
    airplaneModel = glm::rotate(airplaneModel, 30.0f, glm::vec3(1, 0, 0));
    airplaneModel = glm::scale(airplaneModel, glm::vec3(0.001f));
    airplaneModel = glm::translate(airplaneModel, glm::vec3(0.0f, 0.0f, 0.0f));

    // Car model init
    carModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f));
    carModel = glm::rotate(carModel, -90.0f, glm::vec3(1, 0, 0));
    carModel = glm::scale(carModel, glm::vec3(0.04f));

    // Winter house model init 
    winterHouseModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    winterHouseModel = glm::scale(winterHouseModel, glm::vec3(0.1f));

    // snowman model init 
    snowmanModel = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.0f, 0.0f));
    snowmanModel = glm::scale(snowmanModel, glm::vec3(0.38f));
    snowmanModel = glm::translate(snowmanModel, glm::vec3(-4.0f, 0.0f, 0.0f));

    // shovel model init 
    shovelModel = glm::translate(glm::mat4(1.0f), glm::vec3(3.788f, 0.40f, 0.1f));
    shovelModel = glm::scale(shovelModel, glm::vec3(0.45f));
    shovelModel = glm::translate(shovelModel, glm::vec3(-3.4f, -0.5f, -0.1f));

    // ground 3d model init 
    groundObjModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.0f));


	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");




	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(1.0f, 1.0f, 0.0f);
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	//glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

    thunderIntensityLoc = glGetUniformLocation(myBasicShader.shaderProgram, "thunderIntensity");
    fogEnableLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogEnabled");

    // light source model initialization 
    lightSourceModel = glm::translate(glm::mat4(1.0f), lightDir); // Position it at the light source
    lightSourceModel = glm::scale(lightSourceModel, glm::vec3(0.05f)); // Scale it down to make it small


	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //blue moon light 
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    // point light initialization 
    pointLightPosition = glm::vec3(2.0f, 0.0f, 0.0f);
    glm::vec3 pointLightPositionEye = glm::vec3(myCamera.getViewMatrix() * glm::vec4(pointLightPosition, 1.0f));
    pointLightColor = glm::vec3(1.0f, 0.0f, 0.0f);
    pointLightIntensity = 1.0f;
    pointLightRadius = 5.0f;

    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightPosition"), 1, glm::value_ptr(pointLightPositionEye));
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightColor"), 1, glm::value_ptr(pointLightColor));
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightIntensity"), pointLightIntensity);
    glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightRadius"), pointLightRadius);

    // Ground model init ( scroll water )

    groundShader.useShaderProgram();
    groundModel = glm::translate(glm::mat4(1.0f), glm::vec3(12.0f, 1.5f, 0.0f));
    groundModel = glm::rotate(groundModel, glm::radians(180.0f), glm::vec3(0, 0, 1));
    groundModelLoc = glGetUniformLocation(groundShader.shaderProgram, "model");
    glUniformMatrix4fv(groundModelLoc, 1, GL_FALSE, glm::value_ptr(groundModel));
    groundViewLoc = glGetUniformLocation(groundShader.shaderProgram, "view");
    groundProjectionLoc = glGetUniformLocation(groundShader.shaderProgram, "projection");
    glUniformMatrix4fv(groundViewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(groundProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


    // snow model init 
    myBasicShader.useShaderProgram();
    snowModel = glm::translate(glm::mat4(1.0f), glm::vec3(6.5f, 0.2f, 0.0f));
    snowModel = glm::rotate(snowModel, glm::radians(180.0f), glm::vec3(0, 0, 1));
    snowModelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");
    glUniformMatrix4fv(snowModelLoc, 1, GL_FALSE, glm::value_ptr(snowModel));
    snowViewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    snowProjectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    glUniformMatrix4fv(snowViewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(snowProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void renderRain(gps::Shader shader, float deltaTime) {
    shader.useShaderProgram();

    for (auto& rainModel : rainModels) {
        
        rainModel.position.y -= 10.0f * deltaTime; 

       
        if (rainModel.position.y <= -1.0f) {
            rainModel.position.y = static_cast<float>(rand() % 12 + 1); 
            rainModel.position.x = static_cast<float>(rand() % 60 - 30); 
            rainModel.position.z = static_cast<float>(rand() % 60 - 30); 
        }

       
        rainModel.modelMatrix = glm::translate(glm::mat4(1.0f), rainModel.position);
        rainModel.modelMatrix = glm::scale(rainModel.modelMatrix, glm::vec3(0.032f));

        
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(rainModel.modelMatrix));

       
        rain.Draw(shader);
    }
}

void renderSnow(gps::Shader shader, float deltaTime) {
    shader.useShaderProgram();

    int count = 0;
    int maxSnow = 1000;
    for (auto& snowModel : rainModels) {  // Reusing the `rainModels` vector for snow
        if (count >= 1000) break;
        count++;
        //Time-based sine wave for horizontal drift
        float sineWave = sin(glfwGetTime() + snowModel.position.x) * 0.5f; // Oscillation effect
        snowModel.position.x += sineWave * deltaTime;  

        //Simulate falling motion
        snowModel.position.y -= 1.5f * deltaTime;  
        snowModel.position.z += cos(glfwGetTime() + snowModel.position.z) * 0.1f * deltaTime; //Subtle Z-axis drift

       
        if (snowModel.position.y <= -1.0f) {
            snowModel.position.y = static_cast<float>(rand() % 30 + 1);  
            snowModel.position.x = static_cast<float>(rand() % 60 - 30);
            snowModel.position.z = static_cast<float>(rand() % 60 - 30);
        }

       
        snowModel.modelMatrix = glm::translate(glm::mat4(1.0f), snowModel.position);
        snowModel.modelMatrix = glm::scale(snowModel.modelMatrix, glm::vec3(0.05f));

        
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(snowModel.modelMatrix));

        
        rain.Draw(shader);  
    }
}





void renderTeapot(gps::Shader shader) {
    shader.useShaderProgram();

    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    teapot.Draw(shader);
}


glm::mat4 computeLightSpaceTrMatrix() {
    glm::vec3 rotatedLightDir = glm::vec3(lightRotation * glm::vec4(lightDir, 0.0f));
    glm::mat4 lightView = glm::lookAt(rotatedLightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = 0.1f, far_plane = 6.0f;
    glm::mat4 lightProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

    return lightSpaceTrMatrix;
}

void renderCottage(gps::Shader shader, bool depthPass ) {
    shader.useShaderProgram();

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cottageModel));

    if (!depthPass) {
        cottageNormalMatrix = glm::mat3(glm::inverseTranspose(view * cottageModel));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(cottageNormalMatrix));
    }

    cottage.Draw(shader);
}

void renderLightBulb(gps::Shader shader) {

    shader.useShaderProgram();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(lightSourceModel));

    light_bulb.Draw(shader);

}


void renderAirplane(gps::Shader shader, float deltaTime ) {
    if (currentAirplaneX > 50.0f + 30.0f) {
        return; 
    }

    shader.useShaderProgram();

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(airplaneModel));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(airplaneNormalMatrix));

    animateAirplane(currentAirplaneX , 9.0f, -21.0f, 50.0f, deltaTime);
    airplane.Draw(shader);
}

float gravity = 2.0f;
void renderNewObjects(gps::Shader shader, float deltaTime) {
    shader.useShaderProgram();

    for (size_t i = 0; i < newObjectsCreated.size(); i++) {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(newObjectsCreated[i]));

        float x = newObjectsCreated[i][3][0];
        float y = newObjectsCreated[i][3][1];
        float z = newObjectsCreated[i][3][2];

        if (y > 0.01f)
            newObjectsCreated[i][3][1] -= deltaTime * gravity;
        //std::cout << newObjectsCreated[i][3][1] << " ";
        newObject.Draw(shader);
    }
}

float waterTime = 0;
void renderGround(gps::Shader shader, float deltaTime) {
    shader.useShaderProgram();

    waterTime += 0.01;
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "time"), waterTime);
    glUniform2fv(glGetUniformLocation(shader.shaderProgram, "direction"), 1, glm::value_ptr(glm::vec2(1.0f, 0.0f)));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, groundTexture);

    glBindVertexArray(groundVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);  //Render ground with the texture
    glBindVertexArray(0);
}


void renderSnowflake(gps::Shader shader) {
    shader.useShaderProgram();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, snowTexture); 

    glBindVertexArray(snowVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


}

void renderCar(gps::Shader shader) {
    shader.useShaderProgram();

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(carModel));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(carNormalMatrix));

    car.Draw(shader);
}

void renderWinterHouse(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();
    
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(winterHouseModel));

    if (!depthPass) {
        winterHouseNormalMatrix = glm::mat3(glm::inverseTranspose(view * winterHouseModel));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(winterHouseNormalMatrix));
    }

    winterHouse.Draw(shader);
}

void renderGroundObject(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(groundObjModel));

    if (!depthPass) {
        groundObjNormalMatrix = glm::mat3(glm::inverseTranspose(view * groundObjModel));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(groundObjNormalMatrix));
    }
    groundObj.Draw(shader);
}

void renderSnowman(gps::Shader shader, bool depthPass) {
    shader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(snowmanModel));

    if (!depthPass) {
        snowmanNormalMatrix = glm::mat3(glm::inverseTranspose(view * snowmanModel));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(snowmanNormalMatrix));
    }

    snowman.Draw(shader);
}


float rotationAngle = 0.0f; 
void renderShovel(gps::Shader shader, bool depthPass, float deltaTime) {
    shader.useShaderProgram();

    glm::vec3 pivotPoint = glm::vec3(0.0f, -1.0f, 0.0f);

    if (fallShovel) {
       
        rotationAngle += 0.01; 
        if (rotationAngle >= 0.95f) {       
            rotationAngle = 0.8f;
            fallShovel = false;
        }

        shovelModel = glm::translate(shovelModel, pivotPoint);

        shovelModel = glm::rotate(shovelModel, glm::radians(rotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        shovelModel = glm::translate(shovelModel, -pivotPoint);
    }
    //std::cout << "Rotation : " << " " << rotationAngle << std::endl;


    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(shovelModel));


    if (!depthPass) {
        shovelNormalMatrix = glm::mat3(glm::inverseTranspose(view * shovelModel));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(shovelNormalMatrix));
    }

    shovel.Draw(shader);
}

void drawObjects(gps::Shader shader, bool depthPass) {
        renderCottage(shader, depthPass);
        renderGroundObject(shader, depthPass);
}

double lastFrameTime = glfwGetTime();
void renderScene() {
    depthMapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
        1,
        GL_FALSE,
        glm::value_ptr(computeLightSpaceTrMatrix()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    


	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render the scene
    double currentFrameTime = glfwGetTime(); // Get current time
    float deltaTime = float(currentFrameTime - lastFrameTime); //Calculate the difference (time elapsed)
    lastFrameTime = currentFrameTime;

    

    //renderCottage(myBasicShader);
    //renderLightBulb(myBasicShader);
    //renderAirplane(myBasicShader, deltaTime);
    //renderNewObjects(myBasicShader, deltaTime);
    //renderGround(groundShader, deltaTime);
    //renderWinterHouse(myBasicShader);


    //renderCottage(depthMapShader, true);
    //renderGroundObject(depthMapShader, true);
    renderWinterHouse(depthMapShader, true);
    renderSnowman(depthMapShader, true);
    renderShovel(depthMapShader, true, deltaTime);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (showDepthMap) {
        glViewport(0, 0, (float)myWindow.getWindowDimensions().width, (float)myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT);

        screenQuadShader.useShaderProgram();

        //bind the depth map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);
    }
    else {
        glViewport(0, 0, (float)myWindow.getWindowDimensions().width, (float)myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myBasicShader.useShaderProgram();
        
        if (!thunderEnabled) {
            glUniform1f(thunderIntensityLoc, 1);
        }
        else {
            updateThunderIntensity(myBasicShader.shaderProgram);
        }


        // DIRECTIONAL LIGHT UPDATING 
        view = myCamera.getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(lightRotation)) * lightDir));


        // POINT LIGHT UPDATING 


        glm::vec3 pointLightPositions[4] = {
           glm::vec3(-0.038f, 0.316f, 0.069),  //Red
           glm::vec3(-0.382f, 0.316f, 0.069f),  //Green
            glm::vec3(2.55f, 0.186f, 0.10f),  //Blue
            glm::vec3(1.464f, 0.186f, 0.826f)   //Yellow
        };

        glm::vec3 pointLightColors[4] = {
           glm::vec3(1.0f, 0.0f, 0.0f),  //Red
           glm::vec3(0.0f, 1.0f, 0.0f),  //Green
            glm::vec3(0.0f, 0.0f, 1.0f),  //Blue
            glm::vec3(1.0f, 1.0f, 0.0f)   //Yellow
        };

        glm::vec3 pointLightDirections[4] = {
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
        };

        glm::vec3 pointLightPositionEye[4];

        float pointLightIntensities[4] = { 10.0f, 10.8f, 0.6f, 1.0f };
        float pointLightRadiuses[4] = { 0.2f, 0.2f, 2.0f, 0.5f };


        for (int i = 0; i < 4; ++i) {
            pointLightPositionEye[i] = glm::vec3(myCamera.getViewMatrix() * glm::vec4(pointLightPositions[i], 1.0f));
        }

        glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightPositions"), 4, glm::value_ptr(pointLightPositionEye[0]));
        glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightColors"), 4, glm::value_ptr(pointLightColors[0]));
        glUniform1fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightIntensities"), 4, pointLightIntensities);
        glUniform1fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightRadiuses"), 4, pointLightRadiuses);
        glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightDirections"), 4, glm::value_ptr(pointLightDirections[0]));

       /* pointLightPosition = glm::vec3(1.5f, 0.2f, 0.8f);
        glm::vec3 pointLightPositionEye = glm::vec3(myCamera.getViewMatrix() * glm::vec4(pointLightPosition, 1.0f));
        pointLightColor = glm::vec3(1.0f, 0.749f, 0.0f);
        pointLightIntensity = 1.0f;
        pointLightRadius = 3.0f;

        glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightPosition"), 1, glm::value_ptr(pointLightPositionEye));
        glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightColor"), 1, glm::value_ptr(pointLightColor));
        glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightIntensity"), pointLightIntensity);
        glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLightRadius"), pointLightRadius);*/

        //bind the shadow map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
            1,
            GL_FALSE,
            glm::value_ptr(computeLightSpaceTrMatrix()));

        //renderGroundObject(myBasicShader, false);
        //renderCottage(myBasicShader, false);
        renderWinterHouse(myBasicShader, false);
        renderLightBulb(myBasicShader);
        renderNewObjects(myBasicShader, deltaTime);
        renderAirplane(myBasicShader, deltaTime);
        renderSnowman(myBasicShader, false);
        renderShovel(myBasicShader, false, deltaTime);
        renderGround(groundShader, deltaTime);
        if (enableWeather) {
            if (rainOrSnow) {
                renderRain(myBasicShader, deltaTime);
            }
            else
                renderSnow(myBasicShader, deltaTime);

        }
        

        lightShader.useShaderProgram();

        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        lightCubeModel = lightRotation;
        lightCubeModel = glm::translate(lightCubeModel, 1.0f * lightDir);
        lightCubeModel = glm::scale(lightCubeModel, glm::vec3(0.05f, 0.05f, 0.05f));
        //std::cout << "CUBE: " << lightCubeModel[3][0] << " " << lightCubeModel[3][1] << " " << lightCubeModel[3][2] << std::endl;
        //std::cout << "LIGHT ANGLE " << lightAngle << std::endl << std::endl; 
        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        lightCube.Draw(lightShader);
    }

    




    if (isPresentationCamera) {
        //updatePresentationCamera(myCamera, deltaTime, 5.0f);
        updatePresentationCamera(myCamera, deltaTime, cameraChosen);
        glm::mat4 presView = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(presView));
    }
}

void cleanup() {
    myWindow.Delete();
}

int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);



    initOpenGLState();
	initModels();
	initShaders();
	initUniforms();
    initGround();
    generateRain();
    initFBO();
    setWindowCallbacks();

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {


        processMovement();
	    renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
