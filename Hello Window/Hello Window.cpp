#define STB_IMAGE_IMPLEMENTATION
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <cmath>
#include <filesystem>
#include "Shader.h"
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, glm::vec3 *pos, glm::vec3 *front, glm::vec3 *up, float deltaTime);
void checkForShaderErrors(int success, char* logFile, unsigned int shader);
void checkForLinkErrors(int success, char* logFile, unsigned int program);
void loadTextureData(const char* path);
void setTexture2DAttribs();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


int main()
{

/*******************************
* GLFW / GLAD init
********************************/
glfwInit();
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// glfw window creation
GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
if (window == NULL)
{
	std::cout << "Failed to create GLFW window" << std::endl;
	glfwTerminate();
	return -1;
}
glfwMakeContextCurrent(window);

// is called when window is resized
glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
glfwSetCursorPosCallback(window, mouse_callback);
glfwSetScrollCallback(window, scroll_callback);

// glad: load all OpenGL function pointers
// ---------------------------------------
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
{
	std::cout << "Failed to initialize GLAD" << std::endl;
	return -1;
}
glEnable(GL_DEPTH_TEST);

/************************************
* Shdaer init and linking
*************************************/
Shader shader("../Shaders/vertexShader.txt", "../shaders/fragshader.txt");
/**********************************
* vertices setup, VAO & VBO setup
**********************************/

/*

float vert1[] = {
	// positions         // colors		// tex coords
	-0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom left
	0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,// bottom right
	-0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top left
	0.5f, 0.5f, 0.0f, 1.0f, 0.5f, 0.5f, 1.0f, 1.0f    // top right
};
*/
float vert1[] = {
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

glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

// indices for EBO
unsigned int indices[] = {  // note that we start from 0!
	0, 1, 2,   // first triangle
	3, 2, 1    // second triangle
};

float vert2[] = 
{
	-0.5f, -0.5f, 0.0f, // bottom left
	0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, 0.5f, 0.0f    // top left
};

// init VBO, VAO & EBO
unsigned int VBO1, VAO1, VBO2, VAO2, EBO1, TEX1, TEX2;
glGenVertexArrays(1, &VAO1);
glGenBuffers(1, &VBO1);
glBindVertexArray(VAO1);
glGenBuffers(1, &EBO1);

// generate and bind buffer to EBO
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

// generate and bind buffer to VBO
glBindBuffer(GL_ARRAY_BUFFER, VBO1);
glBufferData(GL_ARRAY_BUFFER, sizeof(vert1), vert1, GL_STATIC_DRAW);

// setup and enable vertex attribs
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(1);
//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
//glEnableVertexAttribArray(2);

// bind and load textures

glGenTextures(1, &TEX1);
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, TEX1);
setTexture2DAttribs();
stbi_set_flip_vertically_on_load(true);
loadTextureData("../textures/container.jpg");
glGenTextures(1, &TEX2);
glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, TEX2);
setTexture2DAttribs();
loadTextureData("../textures/awesomeface.jpg");
glBindTexture(GL_TEXTURE_2D, 0);

glBindBuffer(GL_ARRAY_BUFFER, 0);
glBindVertexArray(0);

glm::mat4 trans(1.0f);

float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT;

glm::mat4 view(1);
glm::mat4 projection(1);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
//std::cout << cameraDirection.x << cameraDirection.y << cameraDirection.z << std::endl;
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
//std::cout << cameraRight.x << cameraRight.y << cameraRight.z << std::endl;
//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


// render loop
// -----------
while (!glfwWindowShouldClose(window))
{
	
	// input
	// -----
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	processInput(window, &cameraPos, &cameraFront, &cameraUp, deltaTime);
	shader.use();

	
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.00f);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
		
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TEX1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TEX2);
	glBindVertexArray(VAO1);

	for (unsigned int i= 0; i < 10; i++) {

		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model(1);
		model = glm::translate(model, cubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		shader.setMat4("model", model);
		shader.setInt("tex1", 0);
		shader.setInt("tex2", 1);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	
	//trans = glm::rotate(trans, glm::radians(0.1f), glm::vec3(0.0f, 0.0f, 1.0f));
	//trans = glm::translate(trans, glm::vec3(0.005f, -0.005f, 0.0f));
	
	//shader.setMat4("transform", trans);
	// render
	// ------
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);


	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
}

// glfw: terminate, clearing all previously allocated GLFW resources.
// ------------------------------------------------------------------

glfwTerminate();
return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, glm::vec3 *pos, glm::vec3 *front, glm::vec3 *up, float deltaTime)
{

	float cameraSpeed = 10.0f * deltaTime; // adjust accordingly

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		*pos += cameraSpeed * (*front);
		std::cout << pos->x << " " << pos->y << " " << pos->z << std::endl;
	}
		
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		*pos -= cameraSpeed * (*front);
		std::cout << pos->x << " " << pos->y << " " << pos->z << std::endl;
	}
		
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		*pos -= glm::normalize(glm::cross((*front), (*up))) * cameraSpeed;
		std::cout << pos->x << " " << pos->y << " " << pos->z << std::endl;
	}
		
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		*pos += glm::normalize(glm::cross((*front), (*up))) * cameraSpeed;
		std::cout << pos->x << " " << pos->y << " " << pos->z << std::endl;
	}
		
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void checkForShaderErrors(int success, char* logFile, unsigned int shader) {

	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, logFile);
		std::cout << "ERROR::SHADER COMPILATION FAILED " << logFile << std::endl;

	}

}

void checkForLinkErrors(int success, char* logFile, unsigned int program) {
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success) {

		glGetProgramInfoLog(program, 512, NULL, logFile);
		std::cout << "ERROR::LINK ERROR " << logFile << std::endl;
	}
}

void loadTextureData(const char* path) {

	int h, w, nrChannels;
	unsigned char* data = stbi_load(path, &w, &h, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
	else {
		std::cout << "ERROR::failed to load texture" << std::endl;
	}	
}

void setTexture2DAttribs() {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}


