#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;


struct ShaderProgramSource {
	string VertexSource;
	string FragmentSource;
};
static ShaderProgramSource ParseShader(const string& filePath) {
	enum  class ShaderType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};
	ifstream stream(filePath);
	string line;
	stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != string::npos) {
			if (line.find("vertex") != string::npos) {
				//Set Mode to vertex
				type = ShaderType::VERTEX;
			}
			if (line.find("fragment") != string::npos) {
				//Set Mode to fragment
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << "\n";
		}
	}
	return{ ss[0].str(),ss[1].str() };
}

/// Code to Compile Shader
static unsigned int CompileShader(unsigned int type, const string source) {
	unsigned int id = glCreateShader(type);//Creating Shader
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);//Setting shader source
	glCompileShader(id);//Compiling shader
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);//Getting Shader compile Status
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);//Getting shader compile failure info
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "Shader" : "Fragment") << " shader!" << endl;
		cout << message << endl;
		glDeleteShader(id);//Deleting Shader
		return 0;
	}
	return id;
}

/// Code to Create Shader
static int CreateShader(const string& vertexShader, const string& fragmentShader) {
	unsigned int program = glCreateProgram();//Creating Shader Program
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);//Compiling Vertex Shader
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);//Compiling Fragment Shader
	glAttachShader(program, vs);//Attaching to Vertex Shader
	glAttachShader(program, fs);//Attaching to Fragment Shader
	glLinkProgram(program);//Linking shader to program
	glValidateProgram(program);//Validating shader

	glDeleteShader(vs);//Deleting vertex shader to free memory
	glDeleteShader(fs);//Deleting fragment shader to free memory
	return program;
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	//Initializing GLEW
	if (glewInit() != GLEW_OK) {
		cout << "Error" << endl;
	}
	cout << "GL Version: " << glGetString(GL_VERSION) << endl;
	/* Loop until the user closes the window */

	float positions[] = {
		-0.5f,-0.5f,//0
		0.5f,-0.5f,//1
		0.5f,0.5f,//2
		-0.5f,0.5f,//3
	};
	unsigned int indices[] = {
		0,1,2,
		2,3,0
	};
	//Code to Create Vertex Buffer
	unsigned int buffer;
	glGenBuffers(1, &buffer);//Generating Buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffer);//Binding Buffer
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);//Buffering Data
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	unsigned int indexBuffer;
	glGenBuffers(1, &indexBuffer);//Generating Buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);//Binding Buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);//Buffering Data

	ShaderProgramSource source = ParseShader("Basic.shader");

	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);//Creating shader
	glUseProgram(shader);//Using shader
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		//glDrawArrays(GL_TRIANGLES, 0, 6);//Drawing from Arrays
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	glDeleteProgram(shader);
	glfwTerminate();
	return 0;
}