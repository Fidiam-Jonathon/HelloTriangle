#define STB_IMAGE_IMPLEMENTATION
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <filesystem>
#include "Shader.h"
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void checkForShaderErrors(int success, char* logFile, unsigned int shader);
void checkForLinkErrors(int success, char* logFile, unsigned int program);
void loadTextureData(const char* path);
void setTexture2DAttribs();

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

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

// glad: load all OpenGL function pointers
// ---------------------------------------
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
{
	std::cout << "Failed to initialize GLAD" << std::endl;
	return -1;
}

/************************************
* Shdaer init and linking
*************************************/
Shader shader("../Shaders/vertexShader.txt", "../shaders/fragshader.txt");
/**********************************
* vertices setup, VAO & VBO setup
**********************************/
float vert1[] = {
	// positions         // colors		// tex coords
	-0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom left
	0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,// bottom right
	-0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top left
	0.5f, 0.5f, 0.0f, 1.0f, 0.5f, 0.5f, 1.0f, 1.0f    // top right
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


float texCoords[] =
{
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.5f, 1.0f
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
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(1);
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
glEnableVertexAttribArray(2);

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

shader.use();
shader.setInt("tex1", 0);
shader.setInt("tex2", 1);
shader.setFloat("alpha", 0.0);
float alpha = 0.0;

glm::mat4 trans(1.0f);

//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

// render loop
// -----------
while (!glfwWindowShouldClose(window))
{
	// input
	// -----
	processInput(window);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		alpha += 0.01;
			shader.setFloat("alpha", alpha);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		alpha -= 0.01;
		shader.setFloat("alpha", alpha);
	}
	
	trans = glm::rotate(trans, glm::radians(0.1f), glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::translate(trans, glm::vec3(0.005f, -0.005f, 0.0f));
	
	shader.setMat4("transform", trans);
	// render
	// ------
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TEX1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TEX2);
	glBindVertexArray(VAO1);

	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
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


