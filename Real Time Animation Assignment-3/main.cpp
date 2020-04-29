#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <chrono>
#include <windows.h>
// For reusing constructor
#include <GL/glew.h>
//#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <AntTweakBar.h>
#include<algorithm>

#include "maths_funcs.h"
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>

#include "loader.h"

#include <windows.h> 
#include <mmsystem.h>

int width = 800;
int height = 600;
float zoomVal = 88.5;

GLuint ObjectShaderProgramID;

GLuint objectVAO = 0;
GLuint objectVBO = 0;
GLuint objectloc1;
GLuint objectloc2;

GLfloat translatex = 0.0f;
GLfloat translatey = 0.0f;
GLfloat translatez = 0.0f;

GLfloat rotatex = 0.0f;
GLfloat rotatey = 0.0f;
GLfloat rotatez = 0.0f;


glm::mat4 modelview, projection;
glm::vec3 selected_vertex_value = glm::vec3(0.0f, 0.0f, 0.0f);
int pos = 0;
double file_data[6000];
int file_line_cout = 0;



// Objects
LoadObj neutral_Object("C:/Users/lenovo/source/repos/Real Time Animation Assignment-3/Real Time Animation Assignment-3/Object/High-res Blendshape Model/neutral.obj");
LoadObj smile_r_Object("C:/Users/lenovo/source/repos/Real Time Animation Assignment-3/Real Time Animation Assignment-3/Object/High-res Blendshape Model/Mery_r_smile.obj");
LoadObj smile_l_Object("C:/Users/lenovo/source/repos/Real Time Animation Assignment-3/Real Time Animation Assignment-3/Object/High-res Blendshape Model/Mery_l_smile.obj");
LoadObj sad_r_Object("C:/Users/lenovo/source/repos/Real Time Animation Assignment-3/Real Time Animation Assignment-3/Object/High-res Blendshape Model/Mery_r_sad.obj");
LoadObj sad_l_Object("C:/Users/lenovo/source/repos/Real Time Animation Assignment-3/Real Time Animation Assignment-3/Object/High-res Blendshape Model/Mery_l_sad.obj");
LoadObj eye_closed_r_Object("C:/Users/lenovo/source/repos/Real Time Animation Assignment-3/Real Time Animation Assignment-3/Object/High-res Blendshape Model/Mery_r_eye_closed.obj");
LoadObj eye_closed_l_Object("C:/Users/lenovo/source/repos/Real Time Animation Assignment-3/Real Time Animation Assignment-3/Object/High-res Blendshape Model/Mery_l_eye_closed.obj");
LoadObj puff_r_Object("C:/Users/lenovo/source/repos/Real Time Animation Assignment-3/Real Time Animation Assignment-3/Object/High-res Blendshape Model/Mery_r_puff.obj");
LoadObj puff_l_Object("C:/Users/lenovo/source/repos/Real Time Animation Assignment-3/Real Time Animation Assignment-3/Object/High-res Blendshape Model/Mery_l_puff.obj");
LoadObj brow_lower_r_Object("C:/Users/lenovo/source/repos/Real Time Animation Assignment-3/Real Time Animation Assignment-3/Object/High-res Blendshape Model/Mery_r_brow_lower.obj");
LoadObj brow_lower_l_Object("C:/Users/lenovo/source/repos/Real Time Animation Assignment-3/Real Time Animation Assignment-3/Object/High-res Blendshape Model/Mery_l_brow_lower.obj");

std::vector<float> neutral_Object_Original_Val = neutral_Object.floatVertices;
std::vector<float> smile_r_Object_Original_Val = smile_r_Object.floatVertices;
std::vector<float> smile_l_Object_Original_Val = smile_l_Object.floatVertices;
std::vector<float> sad_r_Object_Original_Val = sad_r_Object.floatVertices;
std::vector<float> sad_l_Object_Original_Val = sad_l_Object.floatVertices;
std::vector<float> eye_closed_r_Object_Original_Val = eye_closed_r_Object.floatVertices;
std::vector<float> eye_closed_l_Object_Original_Val = eye_closed_l_Object.floatVertices;
std::vector<float> puff_r_Object_Original_Val = puff_r_Object.floatVertices;
std::vector<float> puff_l_Object_Original_Val = puff_l_Object.floatVertices;
std::vector<float> brow_lower_r_Object_Original_Val = brow_lower_r_Object.floatVertices;
std::vector<float> brow_lower_l_Object_Original_Val = brow_lower_l_Object.floatVertices;
float smile_r_Object_c_val, smile_l_Object_c_val, sad_r_Object_c_val, sad_l_Object_c_val, eye_closed_r_Object_c_val = 0.0;
float eye_closed_l_Object_c_val, puff_r_Object_c_val, puff_l_Object_c_val, brow_lower_r_Object_c_val, brow_lower_l_Object_c_val = 0.0;


std::string readShaderSource(const std::string& fileName)
{
	std::ifstream file(fileName.c_str());
	if (file.fail()) {
		std::cerr << "Error loading shader called " << fileName << std::endl;
		exit(EXIT_FAILURE);
	}

	std::stringstream stream;
	stream << file.rdbuf();
	file.close();

	return stream.str();
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType) {
	GLuint ShaderObj = glCreateShader(ShaderType);
	if (ShaderObj == 0) {
		std::cerr << "Error creating shader type " << ShaderType << std::endl;
		exit(EXIT_FAILURE);
	}

	/* bind shader source code to shader object */
	std::string outShader = readShaderSource(pShaderText);
	const char* pShaderSource = outShader.c_str();
	glShaderSource(ShaderObj, 1, (const GLchar * *)& pShaderSource, NULL);

	/* compile the shader and check for errors */
	glCompileShader(ShaderObj);
	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		std::cerr << "Error compiling shader type " << ShaderType << ": " << InfoLog << std::endl;
		exit(EXIT_FAILURE);
	}
	glAttachShader(ShaderProgram, ShaderObj); /* attach compiled shader to shader programme */
}

GLuint CompileShaders(const char* pVShaderText, const char* pFShaderText)
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
	GLuint ShaderProgramID = glCreateProgram();
	if (ShaderProgramID == 0) {
		std::cerr << "Error creating shader program" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Create two shader objects, one for the vertex, and one for the fragment shader
	AddShader(ShaderProgramID, pVShaderText, GL_VERTEX_SHADER);
	AddShader(ShaderProgramID, pFShaderText, GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };


	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(ShaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Error linking shader program: " << ErrorLog << std::endl;
		exit(EXIT_FAILURE);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(ShaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(ShaderProgramID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Invalid shader program: " << ErrorLog << std::endl;
		exit(EXIT_FAILURE);
	}
	return ShaderProgramID;
}

void MergingMesh(float smile_r_Object_c, float smile_l_Object_c, float sad_r_Object_c, float sad_l_Object_c, float eye_closed_r_Object_c, float eye_closed_l_Object_c, float puff_r_Object_c, float puff_l_Object_c, float brow_lower_r_Object_c, float brow_lower_l_Object_c) {
	// Merging Mesh
	if (smile_r_Object_c == 0 && smile_l_Object_c == 0 && sad_r_Object_c == 0 && sad_l_Object_c == 0 && eye_closed_r_Object_c == 0 && eye_closed_l_Object_c == 0 && puff_r_Object_c == 0 && puff_l_Object_c == 0 && brow_lower_r_Object_c == 0 && brow_lower_l_Object_c == 0) {
		//std::cout << "MergingMesh() IF Call" << std::endl;
		// Creating a copy of original Values
	}
	else {
		//std::cout << "MergingMesh() ELSE Call" << std::endl;
		// Writing Original Values
		neutral_Object.floatVertices = neutral_Object_Original_Val;
		smile_r_Object.floatVertices = smile_r_Object_Original_Val;
		smile_l_Object.floatVertices = smile_l_Object_Original_Val;
		sad_r_Object.floatVertices = sad_r_Object_Original_Val;
		sad_l_Object.floatVertices = sad_l_Object_Original_Val;
		eye_closed_r_Object.floatVertices = eye_closed_r_Object_Original_Val;
		eye_closed_l_Object.floatVertices = eye_closed_l_Object_Original_Val;
		puff_r_Object.floatVertices = puff_r_Object_Original_Val;
		puff_l_Object.floatVertices = puff_l_Object_Original_Val;
		brow_lower_r_Object.floatVertices = brow_lower_r_Object_Original_Val;
		brow_lower_l_Object.floatVertices = brow_lower_l_Object_Original_Val;


		// smile_r_Object
		std::transform(smile_r_Object.floatVertices.begin(), smile_r_Object.floatVertices.end(), neutral_Object.floatVertices.begin(), smile_r_Object.floatVertices.begin(), std::minus<float>());
		std::transform(smile_r_Object.floatVertices.begin(), smile_r_Object.floatVertices.end(), smile_r_Object.floatVertices.begin(), [&smile_r_Object_c](auto& c) {return c * smile_r_Object_c; });

		// smile_l_Object
		std::transform(smile_l_Object.floatVertices.begin(), smile_l_Object.floatVertices.end(), neutral_Object.floatVertices.begin(), smile_l_Object.floatVertices.begin(), std::minus<float>());
		std::transform(smile_l_Object.floatVertices.begin(), smile_l_Object.floatVertices.end(), smile_l_Object.floatVertices.begin(), [&smile_l_Object_c](auto& c) {return c * smile_l_Object_c; });

		// sad_r_Object
		std::transform(sad_r_Object.floatVertices.begin(), sad_r_Object.floatVertices.end(), neutral_Object.floatVertices.begin(), sad_r_Object.floatVertices.begin(), std::minus<float>());
		std::transform(sad_r_Object.floatVertices.begin(), sad_r_Object.floatVertices.end(), sad_r_Object.floatVertices.begin(), [&sad_r_Object_c](auto& c) {return c * sad_r_Object_c; });

		// sad_l_Object
		std::transform(sad_l_Object.floatVertices.begin(), sad_l_Object.floatVertices.end(), neutral_Object.floatVertices.begin(), sad_l_Object.floatVertices.begin(), std::minus<float>());
		std::transform(sad_l_Object.floatVertices.begin(), sad_l_Object.floatVertices.end(), sad_l_Object.floatVertices.begin(), [&sad_l_Object_c](auto& c) {return c * sad_l_Object_c; });

		// Mery_r_eye_closed
		std::transform(eye_closed_r_Object.floatVertices.begin(), eye_closed_r_Object.floatVertices.end(), neutral_Object.floatVertices.begin(), eye_closed_r_Object.floatVertices.begin(), std::minus<float>());
		std::transform(eye_closed_r_Object.floatVertices.begin(), eye_closed_r_Object.floatVertices.end(), eye_closed_r_Object.floatVertices.begin(), [&eye_closed_r_Object_c](auto& c) {return c * eye_closed_r_Object_c; });

		// Mery_l_eye_closed
		std::transform(eye_closed_l_Object.floatVertices.begin(), eye_closed_l_Object.floatVertices.end(), neutral_Object.floatVertices.begin(), eye_closed_l_Object.floatVertices.begin(), std::minus<float>());
		std::transform(eye_closed_l_Object.floatVertices.begin(), eye_closed_l_Object.floatVertices.end(), eye_closed_l_Object.floatVertices.begin(), [&eye_closed_l_Object_c](auto& c) {return c * eye_closed_l_Object_c; });

		// Mery_r_puff
		std::transform(puff_r_Object.floatVertices.begin(), puff_r_Object.floatVertices.end(), neutral_Object.floatVertices.begin(), puff_r_Object.floatVertices.begin(), std::minus<float>());
		std::transform(puff_r_Object.floatVertices.begin(), puff_r_Object.floatVertices.end(), puff_r_Object.floatVertices.begin(), [&puff_r_Object_c](auto& c) {return c * puff_r_Object_c; });

		// Mery_l_puff
		std::transform(puff_l_Object.floatVertices.begin(), puff_l_Object.floatVertices.end(), neutral_Object.floatVertices.begin(), puff_l_Object.floatVertices.begin(), std::minus<float>());
		std::transform(puff_l_Object.floatVertices.begin(), puff_l_Object.floatVertices.end(), puff_l_Object.floatVertices.begin(), [&puff_l_Object_c](auto& c) {return c * puff_l_Object_c; });

		// Mery_r_brow_lower
		std::transform(brow_lower_r_Object.floatVertices.begin(), brow_lower_r_Object.floatVertices.end(), neutral_Object.floatVertices.begin(), brow_lower_r_Object.floatVertices.begin(), std::minus<float>());
		std::transform(brow_lower_r_Object.floatVertices.begin(), brow_lower_r_Object.floatVertices.end(), brow_lower_r_Object.floatVertices.begin(), [&brow_lower_r_Object_c](auto& c) {return c * brow_lower_r_Object_c; });

		// Mery_l_brow_lower
		std::transform(brow_lower_l_Object.floatVertices.begin(), brow_lower_l_Object.floatVertices.end(), neutral_Object.floatVertices.begin(), brow_lower_l_Object.floatVertices.begin(), std::minus<float>());
		std::transform(brow_lower_l_Object.floatVertices.begin(), brow_lower_l_Object.floatVertices.end(), brow_lower_l_Object.floatVertices.begin(), [&brow_lower_l_Object_c](auto& c) {return c * brow_lower_l_Object_c; });

		// Merging with Neutral
		std::transform(neutral_Object.floatVertices.begin(), neutral_Object.floatVertices.end(), smile_r_Object.floatVertices.begin(), neutral_Object.floatVertices.begin(), std::plus<float>());
		std::transform(neutral_Object.floatVertices.begin(), neutral_Object.floatVertices.end(), smile_l_Object.floatVertices.begin(), neutral_Object.floatVertices.begin(), std::plus<float>());
		std::transform(neutral_Object.floatVertices.begin(), neutral_Object.floatVertices.end(), sad_r_Object.floatVertices.begin(), neutral_Object.floatVertices.begin(), std::plus<float>());
		std::transform(neutral_Object.floatVertices.begin(), neutral_Object.floatVertices.end(), sad_l_Object.floatVertices.begin(), neutral_Object.floatVertices.begin(), std::plus<float>());
		std::transform(neutral_Object.floatVertices.begin(), neutral_Object.floatVertices.end(), eye_closed_r_Object.floatVertices.begin(), neutral_Object.floatVertices.begin(), std::plus<float>());
		std::transform(neutral_Object.floatVertices.begin(), neutral_Object.floatVertices.end(), eye_closed_l_Object.floatVertices.begin(), neutral_Object.floatVertices.begin(), std::plus<float>());
		std::transform(neutral_Object.floatVertices.begin(), neutral_Object.floatVertices.end(), puff_r_Object.floatVertices.begin(), neutral_Object.floatVertices.begin(), std::plus<float>());
		std::transform(neutral_Object.floatVertices.begin(), neutral_Object.floatVertices.end(), puff_l_Object.floatVertices.begin(), neutral_Object.floatVertices.begin(), std::plus<float>());
		std::transform(neutral_Object.floatVertices.begin(), neutral_Object.floatVertices.end(), brow_lower_r_Object.floatVertices.begin(), neutral_Object.floatVertices.begin(), std::plus<float>());
		std::transform(neutral_Object.floatVertices.begin(), neutral_Object.floatVertices.end(), brow_lower_l_Object.floatVertices.begin(), neutral_Object.floatVertices.begin(), std::plus<float>());
	}
}

void generateObjectBuffer(GLuint TempObjectShaderProgramID, float smile_r_Object_c, float smile_l_Object_c, float sad_r_Object_c, float sad_l_Object_c, float eye_closed_r_Object_c, float eye_closed_l_Object_c, float puff_r_Object_c, float puff_l_Object_c, float brow_lower_r_Object_c, float brow_lower_l_Object_c)
{
	//std::cout << "generateObjectBuffer() CALL!! Start" << std::endl;
	objectloc1 = glGetAttribLocation(TempObjectShaderProgramID, "vertex_position");
	objectloc2 = glGetAttribLocation(TempObjectShaderProgramID, "vertex_normals");

	MergingMesh(smile_r_Object_c, smile_l_Object_c, sad_r_Object_c, sad_l_Object_c, eye_closed_r_Object_c, eye_closed_l_Object_c, puff_r_Object_c, puff_l_Object_c, brow_lower_r_Object_c, brow_lower_l_Object_c);

	//neutral_Object.floatVertices.clear();
	GLuint vp_vbo = 0;
	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * neutral_Object.getNumVertices() * sizeof(float), neutral_Object.getVertices(), GL_STATIC_DRAW);
	GLuint vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * neutral_Object.getNumVertices() * sizeof(float), neutral_Object.getNormals(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &objectVAO);
	glBindVertexArray(objectVAO);

	glEnableVertexAttribArray(objectloc1);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(objectloc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(objectloc2);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(objectloc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//std::cout << "generateObjectBuffer() CALL!! End" << std::endl;
}

void init(void) {

	// Toon Reflectance Model
	ObjectShaderProgramID = CompileShaders("C:/Users/lenovo/source/repos/Real Time Animation Assignment-3/Real Time Animation Assignment-3/Shaders/Vertex Shader.vert", "C:/Users/lenovo/source/repos/Real Time Animation Assignment-3/Real Time Animation Assignment-3/Shaders/Fragment Shader.frag");
	generateObjectBuffer(ObjectShaderProgramID, smile_r_Object_c_val, smile_l_Object_c_val, sad_r_Object_c_val, sad_l_Object_c_val, eye_closed_r_Object_c_val, eye_closed_l_Object_c_val, puff_r_Object_c_val, puff_l_Object_c_val, brow_lower_r_Object_c_val, brow_lower_l_Object_c_val);
}

void display() {
	//std::cout << "display() Call!" << std::endl;
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	//glDepthRange(0.0f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(ObjectShaderProgramID);

	int matrix_location = glGetUniformLocation(ObjectShaderProgramID, "model");
	int view_mat_location = glGetUniformLocation(ObjectShaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation(ObjectShaderProgramID, "proj");

	glm::mat4 view_matrix = glm::lookAt(glm::vec3(0.0f, 15.0f, 90.0f), glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 presp_proj_matrix = glm::perspective(zoomVal, (float)(width) / (float)height, 0.1f, 100.0f);
	glm::mat4 model_matrix = glm::mat4(1.0);
	model_matrix = glm::translate(model_matrix, glm::vec3(translatex, translatey, translatez));
	model_matrix = glm::rotate(model_matrix, rotatex, glm::vec3(1, 0, 0));
	model_matrix = glm::rotate(model_matrix, rotatey, glm::vec3(0, 1, 0));
	model_matrix = glm::rotate(model_matrix, rotatez, glm::vec3(0, 0, 1));

	modelview = model_matrix * view_matrix;
	projection = presp_proj_matrix;

	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, glm::value_ptr(presp_proj_matrix));
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(model_matrix));

	glDrawArrays(GL_TRIANGLES, 0, neutral_Object.getNumVertices());
}

//Call back functions
inline void TwEventMouseButtonGLFW3(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		std::cout << "Left Click";
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		std::cout << "Cursor Position: " << x << " , " << y << std::endl;

		GLfloat xpos, ypos, zpos;
		xpos = (float)x;
		ypos = height - y - 1;
		glReadPixels(xpos, ypos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zpos);
		std::cout << "Cursor Position: " << xpos << " , " << ypos << " , " << zpos << std::endl;

		selected_vertex_value = glm::unProject(glm::vec3(xpos, ypos, zpos), modelview, projection, glm::vec4(0.0f, 0.0f, width, height));
		std::cout << "values: " << selected_vertex_value.x << " , " << selected_vertex_value.y << " , " << selected_vertex_value.z << std::endl;

		// finding the near value of vertex selected
		std::vector<vertex> allVertexvalues = neutral_Object.getVerticesData();
		int i;
		float difference = 9999;
		for (i = 0; i < allVertexvalues.size(); i++) {
			float temp = sqrt(((selected_vertex_value.x - allVertexvalues[i].v[0]) * (selected_vertex_value.x - allVertexvalues[i].v[0])) +
				((selected_vertex_value.y - allVertexvalues[i].v[1]) * (selected_vertex_value.y - allVertexvalues[i].v[1])) +
				((selected_vertex_value.z - allVertexvalues[i].v[2]) * (selected_vertex_value.z - allVertexvalues[i].v[2])));
			if (temp < difference) {
				difference = temp;
				pos = i;
			}
		}
		if (difference == 100) {
			std::cout << "Out Of Limit" << std::endl;
			std::cout << "------------------------------------------------------" << std::endl;
		}
		else {
			std::cout << "Position: " << pos + 1 << std::endl;
			selected_vertex_value = glm::vec3(allVertexvalues[pos].v[0], allVertexvalues[pos].v[1], allVertexvalues[pos].v[2]);
			std::cout << "Perfect values: " << selected_vertex_value.x << " , " << selected_vertex_value.y << " , " << selected_vertex_value.z << std::endl;
			std::cout << "------------------------------------------------------" << std::endl;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		// Writing File With new Coordinates;
		std::cout << "Right Click";
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		std::cout << "Cursor Position: " << x << " , " << y << std::endl;

		GLfloat xpos, ypos, zpos;
		xpos = (float)x;
		ypos = height - y - 1;
		glReadPixels(xpos, ypos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zpos);
		std::cout << "Cursor Position: " << xpos << " , " << ypos << " , " << zpos << std::endl;

		glm::vec3 new_location_vertex;
		new_location_vertex = glm::unProject(glm::vec3(xpos, ypos, zpos), modelview, projection, glm::vec4(0.0f, 0.0f, width, height));
		std::cout << "values: " << new_location_vertex.x << " , " << new_location_vertex.y << " , " << new_location_vertex.z << std::endl;

		neutral_Object.floatVertices.clear();
		if (pos != 0) {
			neutral_Object.vertices[pos].v[0] = new_location_vertex.x;
			neutral_Object.vertices[pos].v[1] = new_location_vertex.y;
			neutral_Object.vertices[pos].v[2] = new_location_vertex.z;
			std::cout << neutral_Object.vertices[pos].v[0] << ", " ;
			std::cout << neutral_Object.vertices[pos].v[1] << ", ";
			std::cout << neutral_Object.vertices[pos].v[2] << ", ";
			//neutral_Object.floatVertices[3 * pos] = new_location_vertex.x;
			for (int facesIndex = 0; facesIndex < neutral_Object.faces.size(); facesIndex++) {
				for (int vertex_index = 0; vertex_index < neutral_Object.faces[facesIndex].vertex.size(); vertex_index++) {
					neutral_Object.floatVertices.push_back(neutral_Object.vertices[neutral_Object.faces[facesIndex].vertex[vertex_index]].v[0]);
					neutral_Object.floatVertices.push_back(neutral_Object.vertices[neutral_Object.faces[facesIndex].vertex[vertex_index]].v[1]);
					neutral_Object.floatVertices.push_back(neutral_Object.vertices[neutral_Object.faces[facesIndex].vertex[vertex_index]].v[2]);
				}
			}
			generateObjectBuffer(ObjectShaderProgramID, smile_r_Object_c_val, smile_l_Object_c_val, sad_r_Object_c_val, sad_l_Object_c_val, eye_closed_r_Object_c_val, eye_closed_l_Object_c_val, puff_r_Object_c_val, puff_l_Object_c_val, brow_lower_r_Object_c_val, brow_lower_l_Object_c_val);
		}
	}
	TwEventMouseButtonGLFW(button, action);
}

inline void TwEventMousePosGLFW3(GLFWwindow* window, double xpos, double ypos)
{
	TwMouseMotion(int(xpos), int(ypos));
}

inline void TwEventKeyGLFW3(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
		case(GLFW_KEY_R):
			std::cout << "RESET!";
			smile_r_Object_c_val = 0.0;
			smile_l_Object_c_val = 0.0;
			sad_r_Object_c_val = 0.0;
			sad_l_Object_c_val = 0.0;
			eye_closed_r_Object_c_val = 0.0;
			eye_closed_l_Object_c_val = 0.0;
			puff_r_Object_c_val = 0.0;
			puff_l_Object_c_val = 0.0;
			brow_lower_r_Object_c_val = 0.0;
			brow_lower_l_Object_c_val = 0.0;
			file_line_cout = 0;
			neutral_Object.floatVertices = neutral_Object_Original_Val;
			generateObjectBuffer(ObjectShaderProgramID, smile_r_Object_c_val, smile_l_Object_c_val, sad_r_Object_c_val, sad_l_Object_c_val, eye_closed_r_Object_c_val, eye_closed_l_Object_c_val, puff_r_Object_c_val, puff_l_Object_c_val, brow_lower_r_Object_c_val, brow_lower_l_Object_c_val);
			break;
		case(GLFW_KEY_A):
			//std::cout << "AUTO! Line Count: " << file_line_cout << std::endl;

			while(file_line_cout <= 6000){
				std::cout << "+++++++++++++++++++++++++++" << file_line_cout << std::endl;
				smile_r_Object_c_val = file_data[file_line_cout + 1];
				smile_l_Object_c_val = file_data[file_line_cout + 12];
				sad_r_Object_c_val = file_data[file_line_cout + 2];
				sad_l_Object_c_val = file_data[file_line_cout + 13];
				eye_closed_r_Object_c_val = file_data[file_line_cout + 7];
				eye_closed_l_Object_c_val = file_data[file_line_cout + 18];
				puff_r_Object_c_val = file_data[file_line_cout + 3];
				puff_l_Object_c_val = file_data[file_line_cout + 14];
				brow_lower_r_Object_c_val = file_data[file_line_cout + 10];
				brow_lower_l_Object_c_val = file_data[file_line_cout + 21];
				generateObjectBuffer(ObjectShaderProgramID, smile_r_Object_c_val, smile_l_Object_c_val, sad_r_Object_c_val, sad_l_Object_c_val, eye_closed_r_Object_c_val, eye_closed_l_Object_c_val, puff_r_Object_c_val, puff_l_Object_c_val, brow_lower_r_Object_c_val, brow_lower_l_Object_c_val);
				display();
				TwDraw();
				glfwSwapBuffers(window);
				file_line_cout += 24;
			}
			std::cout << "End of File!";
			break;
		case(GLFW_KEY_S):
			zoomVal += 0.1;
			break;
		case(GLFW_KEY_X):
			zoomVal -= 0.1;
			std::cout << "zoomVal: " << zoomVal;
			break;
		}
	}
	TwEventKeyGLFW(key, action);
}

inline void TwWindowSizeGLFW3(GLFWwindow* window, int width, int height)
{
	TwWindowSize(width, height);
}

void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void TW_CALL setLEyeCloseCB(const void* value, void* clientData)
{
	*(float*)clientData = *(const float*)value;
	smile_l_Object_c_val = smile_r_Object_c_val;
	sad_l_Object_c_val = sad_r_Object_c_val;
	eye_closed_l_Object_c_val = eye_closed_r_Object_c_val;
	puff_l_Object_c_val = puff_r_Object_c_val;
	brow_lower_l_Object_c_val = brow_lower_r_Object_c_val;
	std::cout << "smile_Object_c_val: " << smile_r_Object_c_val << " sad_Object_c_val: " << sad_r_Object_c_val << "eye_closed_Object_c_val: ";
	std::cout << eye_closed_r_Object_c_val << "puff_Object_c_val: " << puff_r_Object_c_val << "brow_lower_Object_c_val: " << brow_lower_r_Object_c_val << std::endl;
	generateObjectBuffer(ObjectShaderProgramID, smile_r_Object_c_val, smile_l_Object_c_val, sad_r_Object_c_val, sad_l_Object_c_val, eye_closed_r_Object_c_val, eye_closed_l_Object_c_val, puff_r_Object_c_val, puff_l_Object_c_val, brow_lower_r_Object_c_val, brow_lower_l_Object_c_val);
}

void TW_CALL getLEyeCloseCB(void* value, void* clientData)
{
	*(float*)value = *(float*)clientData;
}

void TW_CALL RunCB(void* /*clientData*/)
{
	//Gump.State = RUNNING;
	std::cout << "Button Callback!";
}

int main(int argc, char** argv) {
	std::ifstream input("C:/Users/lenovo/source/repos/Real Time Animation Assignment-3/Real Time Animation Assignment-3/blendshape_animation.txt");

	for (int i = 0; i < 6000; i++) {
		input >> file_data[i];
	}

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	// AntTweakBar
	// Reference: http://anttweakbar.sourceforge.net/doc/tools:anttweakbar:howto
	TwBar* myBar;
	GLFWwindow* window = glfwCreateWindow(width, height, "Real-Time Animation Assignment 3", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // can be GLFW_CURSOR_HIDDEN

	// AntTweakBar
	TwInit(TW_OPENGL_CORE, NULL);
	myBar = TwNewBar("TweakBar Menu");
	TwWindowSize(800, 600);
	TwDefine(" GLOBAL help='Integrating AntTweakBar with GLFW and OpenGL.' ");
	// Lariables Values
	TwAddVarRO(myBar, "Width", TW_TYPE_INT32, &width, " label='Wnd width' help='Width of the graphics window (in pixels)' ");
	TwAddVarRO(myBar, "Height", TW_TYPE_INT32, &height, " label='Wnd height' help='Height of the graphics window (in pixels)' ");
	TwAddVarRO(myBar, "Vertex_x Values", TW_TYPE_FLOAT, &selected_vertex_value.x, " label='Vertex_x' help='value of vertex at x' ");
	TwAddVarRO(myBar, "Vertex_y Values", TW_TYPE_FLOAT, &selected_vertex_value.y, " label='Vertex_y' help='value of vertex at y' ");
	TwAddVarRO(myBar, "Vertex_z Values", TW_TYPE_FLOAT, &selected_vertex_value.z, " label='Vertex_z' help='value of vertex at z' ");
	// Change values of variables
	TwAddVarCB(myBar, "Smile", TW_TYPE_FLOAT, setLEyeCloseCB, getLEyeCloseCB, &smile_r_Object_c_val, "min=0 max=1 step=0.1 label='Smile Mesh'");
	TwAddVarCB(myBar, "Sad", TW_TYPE_FLOAT, setLEyeCloseCB, getLEyeCloseCB, &sad_r_Object_c_val, "min=0 max=1 step=0.1 label='Sad Mesh'");
	TwAddVarCB(myBar, "EYE", TW_TYPE_FLOAT, setLEyeCloseCB, getLEyeCloseCB, &eye_closed_r_Object_c_val, "min=0 max=1 step=0.1 label='EYE Mesh'");
	TwAddVarCB(myBar, "PUFF", TW_TYPE_FLOAT, setLEyeCloseCB, getLEyeCloseCB, &puff_r_Object_c_val, "min=0 max=1 step=0.1 label='PUFF Mesh'");
	TwAddVarCB(myBar, "BROW", TW_TYPE_FLOAT, setLEyeCloseCB, getLEyeCloseCB, &brow_lower_r_Object_c_val, "min=0 max=1 step=0.1 label='BROW Mesh'");
	
	// Set GLFW event callbacks
	glfwSetWindowSizeCallback(window, (GLFWwindowposfun)TwWindowSizeGLFW3);
	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW3);
	glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW3);
	glfwSetKeyCallback(window, (GLFWkeyfun)TwEventKeyGLFW3);
	// Initialize GLEW.
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (err != GLEW_OK) {
		fprintf(stderr, "Error: %s.\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	init();

	while (!glfwWindowShouldClose(window))
	{
		glfwWaitEvents();
		display();
		TwDraw();
		glfwSwapBuffers(window);
	}
	TwTerminate();
	glfwTerminate();
	return 0;
}