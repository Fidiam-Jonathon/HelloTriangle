
#include <glad\glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void checkForShaderErrors(int success, char* logFile, unsigned int shader);
void checkForLinkErrors(int success, char* logFile, unsigned int program);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// shader sources

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
"}\0";
const char *fragShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

const char *fragShaderSourceYellow = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
"}\n\0";



int main()
{

/*******************************
* GLFW / GLAD init
********************************/
glfwInit();
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif									 // glfw window creation
													 // --------------------
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


unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
unsigned int yellowShader = glCreateShader(GL_FRAGMENT_SHADER);
glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
glCompileShader(vertexShader);
glShaderSource(fragShader, 1, &fragShaderSource, NULL);
glCompileShader(fragShader);
glShaderSource(yellowShader,1,&fragShaderSourceYellow, NULL);
glCompileShader(yellowShader);


int success;
char shaderLog[512];

glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
checkForShaderErrors(success, shaderLog, vertexShader);

glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
checkForShaderErrors(success, shaderLog, fragShader);

glGetShaderiv(yellowShader, GL_COMPILE_STATUS, &success);
checkForShaderErrors(success, shaderLog, yellowShader);

// link shaders
unsigned int shaderProgram = glCreateProgram();
unsigned int shaderProgram1 = glCreateProgram();
glAttachShader(shaderProgram, vertexShader);
glAttachShader(shaderProgram, fragShader);
glLinkProgram(shaderProgram);

glAttachShader(shaderProgram1, vertexShader);
glAttachShader(shaderProgram1, yellowShader);
glLinkProgram(shaderProgram1);

checkForLinkErrors(success, shaderLog, shaderProgram);

glDeleteShader(vertexShader);
glDeleteShader(fragShader);

/**********************************
* vertices setup, VAO & VBO setup
**********************************/
float vert1[] =
{
	-0.75, 0.5f, 0.0f, // 1st tri far left
	0.0f, -0.5f, 0.0f, // 1st tri center right
	-0.75f, -0.5f, 0.0f, // 1st tri bottom right

};

float vert2[] = 
{
	-0.75f, -0.5f, 0.0f, // 2nd tri bottom left
	0.75f, -0.5f, 0.0f,  // 2nd tri bottom right
	0.75f, 0.5f, 0.0f    // 2nd tri right top
};

// indices for EBO
unsigned int indices[] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	2, 1, 3    // second triangle
};

// init VBO, VAO & EBO
unsigned int VBO1, VAO1, VBO2, VAO2;
glGenVertexArrays(1, &VAO1);
glGenBuffers(1, &VBO1);
glBindVertexArray(VAO1);
//glGenBuffers(1, &EBO1);

// generate and bind buffer to EBO
//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

// generate and bind buffer to VBO
glBindBuffer(GL_ARRAY_BUFFER, VBO1);
glBufferData(GL_ARRAY_BUFFER, sizeof(vert1), vert1, GL_STATIC_DRAW);


// setup and enable vertex attribs
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

glEnableVertexAttribArray(0);
glBindBuffer(GL_ARRAY_BUFFER, 0);
glBindVertexArray(0);
//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

// init 2nd VBO & VAO
glGenVertexArrays(1, &VAO2);
glGenBuffers(1, &VBO2);
glBindVertexArray(VAO2);

glBindBuffer(GL_ARRAY_BUFFER, VBO2);
glBufferData(GL_ARRAY_BUFFER, sizeof(vert2), vert2, GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
glBindBuffer(GL_ARRAY_BUFFER, 0);
glBindVertexArray(0);

// render loop
// -----------
while (!glfwWindowShouldClose(window))
{
	// input
	// -----
	processInput(window);

	// render
	// ------
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgram);
	glBindVertexArray(VAO1);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glUseProgram(shaderProgram1);
	glBindVertexArray(VAO2);
	glDrawArrays(GL_TRIANGLES, 0, 3);
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


