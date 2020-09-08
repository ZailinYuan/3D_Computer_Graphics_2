#include <iostream>
#include <string>
#include <vector>
#include <typeinfo>
#include<limits>

// GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other includes
#include "Shader.h"
#include "nanogui/nanogui.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Self defined:
#include "Camera.hpp"
#include "PointLight.hpp"
#include "DirectionLight.hpp"

using namespace nanogui;
using namespace std;

enum test_enum {
	Item1 = 0,
	Item2,
	Item3
}; 
test_enum enumval = Item2;

enum texture_enum {
	texture1 = 0,
	texture2 = 1,
	texture3 = 2,
}; 
texture_enum enumTextures = texture1;

enum culling_enum {
	CW, CCW,
};
culling_enum clock = CW;

enum surface_enum {
	FLAT, SMOOTH,
};
surface_enum sur = SMOOTH;

enum depth_enum {
	ALWAYS,
	NEVER,
	LESS,
	EQUAL,
	LEQUAL,
	GREATER,
	NOTEQUAL,
	GEQUAL,
};
depth_enum depth = LESS;

struct LightController {
	bool dirLight = false;
	bool pointLight = false;
	bool pointRotX = false;
	bool pointRotY = false;
	bool pointRotZ = false;
}; 
LightController lightController;

struct ObjectController {
	bool needSpin = false;
	bool needRoll = false;
};
ObjectController objectController;

struct TextureController {
	bool textureStatus = false;
	bool normalMapStatus = false;
};
TextureController textureController;

// Texture media:
unsigned int diffTexture;
unsigned int normTexture;

// Ambient Strength:
float ambientSetting = 0.3f;

// Window dimensions
const GLuint width = 800, height = 600;

// VAO, VBO:
GLuint VBO, VAO;

// Light position:
glm::vec3 lightPos(0.0f, 0.0f, 6.0f);

// Material:
float shininess = 32;
float specularIntensity = 1.0f;

// Object color:
Color objectColor = Color(1.0f, 1.0f, 1.0f, 1.f);
// Color objectColor = Color(0.5f, 0.5f, 0.7f, 1.f);
// Object roll control:
float rollUnit = 0.0f;		// Default no roll.
float spinUnit = 0.0f;		// Default no roll.

// Point light:
PointLight* p1 = new PointLight();

// Direction light:
DirectionLight* d1 = new DirectionLight();

// Default model:
string model = "cube.obj";

// Global Objects:
Screen* screen = nullptr;
Camera* c = new Camera(d1);


// Projection settings:
float fov = 45.0f;
float zNear = 0.1f;
float zFar = 10.0f;

// Core structure:
struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

// Object Buffer:
vector<Vertex> vs;
// Buffer size (how many vertices):
int length = 0;

// Fucntions:
void load_obj(string obj_path, vector<Vertex>& vertices);
unsigned int load_texture(const char* theTexture, bool rgba);
void setCallbacks(GLFWwindow* window);
void initGui();
int getModel(vector<Vertex>& vs, string model);
void setBasicGui(Screen* screen);
void setCameraGui(Screen* screen);
void setLightGui(Screen* screen);

// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init gui:
	initGui();

	// Create a GLFWwindow object
	GLFWwindow* window = glfwCreateWindow(width, height, "3D Computer Graphics", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// 绑定上下文与window对象，不然while死循环中颜色显示不在window上。
	glfwMakeContextCurrent(window);


	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glfwSwapInterval(0);
	glfwSwapBuffers(window);

	// Create a nanogui screen and pass the glfw pointer to initialize
	screen = new Screen();
	screen->initialize(window, true);

	// Create nanogui gui
	setBasicGui(screen);
	setLightGui(screen);
	setCameraGui(screen);

	// Display screen:
	screen->setVisible(true);
	screen->performLayout();

	// Add callbacks:
	setCallbacks(window);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Build and compile our shader program
	Shader ourShader("./shader/basic.vert", "./shader/basic.frag");

	// Load model
	length = getModel(vs, model);

	// Load texture
	diffTexture = load_texture("textures/cube_diffuse.png", true);
	normTexture = load_texture("textures/cube_normal.png", false);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Clear the colorbuffer 填充整个区域
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // 前三位是RGB，后一位是X度
		glClearDepth(1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// Read data from files:
		//length = getModel(vs, model);
		glEnable(GL_DEPTH_TEST);

		// Depth:
		if (depth == ALWAYS) {
			glDepthFunc(GL_ALWAYS);
		}
		else if (depth == LESS) {
			glDepthFunc(GL_LESS);
		}
		else if(depth == NEVER){
			glDepthFunc(GL_NEVER);
		}
		else if (depth == EQUAL) {
			glDepthFunc(GL_EQUAL);
		}
		else if (depth == LEQUAL) {
			glDepthFunc(GL_LEQUAL);
		}
		else if (depth == GREATER) {
			glDepthFunc(GL_GREATER);
		}
		else if (depth == NOTEQUAL) {
			glDepthFunc(GL_NOTEQUAL);
		}
		else if (depth == GEQUAL) {
			glDepthFunc(GL_GEQUAL);
		}

		// Object transportations:
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, rollUnit * glm::radians(50.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, spinUnit * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 0.3f * glm::radians(50.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		if (objectController.needSpin) {
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (objectController.needRoll) {
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		}
		glm::mat4 view = c->camLookAt();
		glm::mat4 projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, zNear, zFar);


		// Set front / back:
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);

		if (clock == CW) {
			glFrontFace(GL_CW);
		}
		else if (clock == CCW) {
			glFrontFace(GL_CCW);
		}

		// bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normTexture);

		// Render:
		ourShader.use();
		glBindVertexArray(VAO);
		glUniform1i(glGetUniformLocation(ourShader.program, "diffTexture"), 0);
		glUniform1i(glGetUniformLocation(ourShader.program, "normTexture"), 1);

		// Set model, view, projection matrices:
		int modelLoc = glGetUniformLocation(ourShader.program, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		int viewLoc = glGetUniformLocation(ourShader.program, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		int projectLoc = glGetUniformLocation(ourShader.program, "projection");
		glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Set view position:
		int viewPos = glGetUniformLocation(ourShader.program, "viewPos");
		glUniform3f(viewPos, p1->lightPosition[0], p1->lightPosition[1], p1->lightPosition[2]);

		/////// temp: lightPos:
		int lightPos = glGetUniformLocation(ourShader.program, "lightPos");
		glUniform3f(lightPos, c->cameraPos[0], c->cameraPos[1], c->cameraPos[2]);

		// Set environment parameters:
		int ambientStr = glGetUniformLocation(ourShader.program, "environ.ambientStrength");
		glUniform1f(ambientStr, ambientSetting);

		// Set material properties:
		int shiny = glGetUniformLocation(ourShader.program, "material.shininess");
		glUniform1f(shiny, shininess);

		int specularStr = glGetUniformLocation(ourShader.program, "material.specularStrength");
		glUniform1f(specularStr, specularIntensity);

		// Set object color:
		int objectColorPick = glGetUniformLocation(ourShader.program, "objectColor");
		glUniform3f(objectColorPick, objectColor[0], objectColor[1], objectColor[2]);

		// Set controls:
		int textureStatusControl = glGetUniformLocation(ourShader.program, "diffuseStatus");
		glUniform1i(textureStatusControl, textureController.textureStatus);

		int normalStatusControl = glGetUniformLocation(ourShader.program, "normalStatus");
		glUniform1i(normalStatusControl, textureController.normalMapStatus);

		// Set point lights:
		if (lightController.pointRotX == true) {
			p1->rotateX();
		}
		if (lightController.pointRotY == true) {
			p1->rotateY();
		}
		if (lightController.pointRotZ == true) {
			p1->rotateZ();
		}

		int pointLightPos = glGetUniformLocation(ourShader.program, "pointLight.position");
		glUniform3f(pointLightPos, p1->lightPosition[0], p1->lightPosition[1], p1->lightPosition[2]);

		if (lightController.pointLight) {
			int pointLightA = glGetUniformLocation(ourShader.program, "pointLight.ambient");
			glUniform3f(pointLightA, p1->ambientColor[0], p1->ambientColor[1], p1->ambientColor[2]);

			int pointLightD = glGetUniformLocation(ourShader.program, "pointLight.diffuse");
			glUniform3f(pointLightD, p1->diffuseColor[0], p1->diffuseColor[1], p1->diffuseColor[2]);

			int pointLightS = glGetUniformLocation(ourShader.program, "pointLight.specular");
			glUniform3f(pointLightS, p1->specularColor[0], p1->specularColor[1], p1->specularColor[2]);
		}
		else {
			int pointLightA = glGetUniformLocation(ourShader.program, "pointLight.ambient");
			glUniform3f(pointLightA, p1->ambientBlack[0], p1->ambientBlack[1], p1->ambientBlack[2]);

			int pointLightD = glGetUniformLocation(ourShader.program, "pointLight.diffuse");
			glUniform3f(pointLightD, p1->diffuseBlack[0], p1->diffuseBlack[1], p1->diffuseBlack[2]);

			int pointLightS = glGetUniformLocation(ourShader.program, "pointLight.specular");
			glUniform3f(pointLightS, p1->specularBlack[0], p1->specularBlack[1], p1->specularBlack[2]);
		}
		
		// Set direct lights:
		int dirLightPos = glGetUniformLocation(ourShader.program, "dirLight.direction");
		glUniform3f(dirLightPos, d1->lightDirection[0], d1->lightDirection[1], d1->lightDirection[2]);

		if (lightController.dirLight == true) {
			int dirLightA = glGetUniformLocation(ourShader.program, "dirLight.ambient");
			glUniform3f(dirLightA, d1->ambientColor[0], d1->ambientColor[1], d1->ambientColor[2]);

			int dirLightD = glGetUniformLocation(ourShader.program, "dirLight.diffuse");
			glUniform3f(dirLightD, d1->diffuseColor[0], d1->diffuseColor[1], d1->diffuseColor[2]);

			int dirLightS = glGetUniformLocation(ourShader.program, "dirLight.specular");
			glUniform3f(dirLightS, d1->specularColor[0], d1->specularColor[1], d1->specularColor[2]);
		}
		else {
			int dirLightA = glGetUniformLocation(ourShader.program, "dirLight.ambient");
			glUniform3f(dirLightA, d1->ambientBlack[0], d1->ambientBlack[1], d1->ambientBlack[2]);

			int dirLightD = glGetUniformLocation(ourShader.program, "dirLight.diffuse");
			glUniform3f(dirLightD, d1->diffuseBlack[0], d1->diffuseBlack[1], d1->diffuseBlack[2]);

			int dirLightS = glGetUniformLocation(ourShader.program, "dirLight.specular");
			glUniform3f(dirLightS, d1->specularBlack[0], d1->specularBlack[1], d1->specularBlack[2]);
		}

		if (sur == SMOOTH) {
			int flat = glGetUniformLocation(ourShader.program, "f");
			glUniform1i(flat, true);
		}
		else if (sur == FLAT) {
			int flat = glGetUniformLocation(ourShader.program, "f");
			glUniform1i(flat, false);
		}

		// Draw the triangle
		if (enumTextures == texture1) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, length);  // 告诉我们要画三角，0代表从VBO中 0 位置读取，3 代表连续 3 个
		}
		else if (enumTextures == texture2) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLES, 0, length);  // 告诉我们要画三角，0代表从VBO中 0 位置读取，3 代表连续 3 个
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINTS);
			glDrawArrays(GL_POINTS, 0, length);  // 告诉我们要画三角，0代表从VBO中 0 位置读取，3 代表连续 3 个
		}

		glBindVertexArray(0);

		screen->drawWidgets();

		// Swap the screen buffers
		glfwSwapBuffers(window);
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

unsigned int load_texture(const char* theTexture, bool rgba) {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load texture
	int TexWidth, TexHeight, nrChannels;
	unsigned char* data = stbi_load(theTexture, &TexWidth, &TexHeight, &nrChannels, 0);
	if (data) {
		if (rgba) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TexWidth, TexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TexWidth, TexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		cout << "Error: failed loading texture" << endl;
	}
	stbi_image_free(data);
	
	return texture;
}

void setLightGui(Screen* screen) {
	FormHelper* gui = new FormHelper(screen);
	nanogui::ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Light Controller");

	gui->addGroup("Lighting");
	gui->addVariable("Object color", objectColor);
	gui->addVariable("Shininess", shininess)->setMinValue(1);
	gui->addVariable("ambientStrength", ambientSetting)->setSpinnable(true);
	gui->addVariable("Direction light status", lightController.dirLight);
	gui->addVariable("Direction light x direction", d1->lightDirection[0])->setSpinnable(true);
	gui->addVariable("Direction light y direction", d1->lightDirection[1])->setSpinnable(true);
	gui->addVariable("Direction light z direction", d1->lightDirection[2])->setSpinnable(true);
	gui->addVariable("Direction light ambient color", d1->ambientColor);
	gui->addVariable("Direction light diffuse color", d1->diffuseColor);
	gui->addVariable("Direction light specular color", d1->specularColor);
	gui->addVariable("Point light status", lightController.pointLight);
	gui->addVariable("Point light ambient color", p1->ambientColor);
	gui->addVariable("Point light diffuse color", p1->diffuseColor);
	gui->addVariable("Point light specular color", p1->specularColor);
	gui->addVariable("Rotate speed", p1->revolutionSpeed)->setSpinnable(true);
	gui->addVariable("Point light rotate on X", lightController.pointRotX);
	gui->addVariable("Point light rotate on Y", lightController.pointRotY);
	gui->addVariable("Point light rotate on Z", lightController.pointRotZ);

	gui->addGroup("Texture");
	gui->addVariable("Texture status", textureController.textureStatus);
	gui->addVariable("Normal map status", textureController.normalMapStatus);

	gui->addButton("Reset point light", []() {
		p1->reset();
		lightController.pointRotX = false;
		lightController.pointRotY = false;
		lightController.pointRotZ = false;
	});
}

void setCameraGui(Screen* screen) {
	FormHelper* gui = new FormHelper(screen);
	nanogui::ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Camera Controller");

	gui->addGroup("Position");
	gui->addVariable("Camera X", c->cameraPos[0])->setSpinnable(true);
	gui->addVariable("Camera Y", c->cameraPos[1])->setSpinnable(true);
	gui->addVariable("Camera Z", c->cameraPos[2])->setSpinnable(true);

	gui->addGroup("Rotate");
	gui->addVariable("Rotate unit", c->rotateUnit)->setSpinnable(true);
	gui->addButton("Rotate right+", []() { c->rotateRightPlus(); });
	gui->addButton("Rotate right-", []() { c->rotateRightMinus(); });
	gui->addButton("Rotate up+", []() { c->rotateUpPlus(); });
	gui->addButton("Rotate up-", []() { c->rotateUpMinus(); });
	gui->addButton("Rotate front+", []() { c->rotateFrontPlus(); });
	gui->addButton("Rotate front-", []() { c->rotateFrontMinus(); });

	gui->addGroup("Revolution");
	gui->addVariable("Revolution rate", c->revolutionRate)->setSpinnable(true);
	gui->addButton("Revolution horizontal +", []() { c->revolutionHorizontalPlus(); });
	gui->addButton("Revolution horizontal -", []() { c->revolutionHorizontalMinus(); });
	gui->addButton("Revolution Vertical +", []() { c->revolutionVerticalPlus(); });
	gui->addButton("Revolution Vertical -", []() { c->revolutionVerticalMinus(); });

	gui->addButton("Reset", []() { c->reset(); });
}

void setBasicGui(Screen* screen) {
	bool enabled = true;
	FormHelper* gui = new FormHelper(screen);
	nanogui::ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Basic Settings");

	gui->addGroup("Object control");
	gui->addVariable("Spin", objectController.needSpin);
	gui->addVariable("Roll", objectController.needRoll);
	gui->addButton("Roll +", []() { rollUnit = rollUnit + 0.1f; });
	gui->addButton("Roll -", []() { rollUnit = rollUnit - 0.1f; });
	gui->addButton("Spin +", []() { spinUnit = spinUnit + 0.1f; });
	gui->addButton("Spin -", []() { spinUnit = spinUnit - 0.1f; });

	gui->addGroup("Configration");
	gui->addVariable("Z near", zNear)->setSpinnable(true);
	gui->addVariable("Z far", zFar)->setSpinnable(true);
	gui->addVariable("Render type", enumTextures, enabled)->setItems({ "Triangle", "Line", "Point" });
	gui->addVariable("Culling type", clock, enabled)->setItems({ "CW", "CCW" });
	gui->addVariable("Shading type", sur, enabled)->setItems({ "SMOOTH", "FLAT" });
	gui->addVariable("Model name", model);
	gui->addVariable("Depth type", depth)->setItems({ "ALWAYS", "NEVER", "LESS", "EQUAL", "LEQUAL", "GREATER", "NOTEQUAL", "GEQUAL" });
	gui->addButton("Reload model", []() { 
		length = getModel(vs, model);
		if (model == "cube.obj") {
			diffTexture = load_texture("textures/cube_diffuse.png", true);
			normTexture = load_texture("textures/cube_normal.png", false);
		}
		else if (model == "cyborg.obj") {
			diffTexture = load_texture("textures/cyborg_diffuse.png", false);
			normTexture = load_texture("textures/cyborg_normal.png", false);
		}
	});

	gui->addButton("Reset", []() { 
		// todo
	});
}

void load_obj(string obj_path, vector<Vertex>& vertices) {
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> tex_coods;
	ifstream ifs;

	try {
		ifs.open(obj_path);
		string one_line;
		int max = INT_MIN;
		int min = INT_MAX;
		while (getline(ifs, one_line)) {
			stringstream ss(one_line);
			string type;
			ss >> type;
			if (type == "v") {
				glm::vec3 vert_pos;
				ss >> vert_pos[0] >> vert_pos[1] >> vert_pos[2];
				max = max < vert_pos[1] ? vert_pos[1] : max;
				min = min > vert_pos[1] ? vert_pos[1] : min;
				positions.push_back(vert_pos);
			}
			else if (type == "vt") {
				glm::vec2 tex_coord;
				ss >> tex_coord[0] >> tex_coord[1];
				tex_coods.push_back(tex_coord);
			}
			else if (type == "vn") {
				glm::vec3 vert_norm;
				ss >> vert_norm[0] >> vert_norm[1] >> vert_norm[2];
				normals.push_back(vert_norm);
			}
			else if (type == "f") {
				string s_vertex_0, s_vertex_1, s_vertex_2;
				ss >> s_vertex_0 >> s_vertex_1 >> s_vertex_2;
				int pos_idx, tex_idx, norm_idx;
				sscanf(s_vertex_0.c_str(), "%d/%d/%d", &pos_idx, &tex_idx, &norm_idx);
				// We have to use index -1 because the obj index starts at 1
				Vertex vertex_0;
				vertex_0.Position = positions[pos_idx - 1];
				vertex_0.TexCoords = tex_coods[tex_idx - 1];
				vertex_0.Normal = normals[norm_idx - 1];
				sscanf(s_vertex_1.c_str(), "%d/%d/%d", &pos_idx, &tex_idx, &norm_idx);
				Vertex vertex_1;
				vertex_1.Position = positions[pos_idx - 1];
				vertex_1.TexCoords = tex_coods[tex_idx - 1];
				vertex_1.Normal = normals[norm_idx - 1];
				sscanf(s_vertex_2.c_str(), "%d/%d/%d", &pos_idx, &tex_idx, &norm_idx);
				Vertex vertex_2;
				vertex_2.Position = positions[pos_idx - 1];
				vertex_2.TexCoords = tex_coods[tex_idx - 1];
				vertex_2.Normal = normals[norm_idx - 1];
				vertices.push_back(vertex_0);
				vertices.push_back(vertex_1);
				vertices.push_back(vertex_2);
			}
		}

		float mid = (max + min) / 2;
		for (unsigned long long i = 0; i < vertices.size(); ++i) {
			vertices[i].Position[1] -= mid;
		}

		/*for (unsigned long long i = 0; i < vertices.size(); i += 3) {
			cout << vertices[i].Position[0] << " " << vertices[i].Position[1] << " " << vertices[i].Position[2] << endl;
			cout << vertices[i].Normal[0] << " " << vertices[i].Normal[1] << " " << vertices[i].Normal[2] << endl;
			cout << vertices[i].TexCoords[0] << " " << vertices[i].TexCoords[1] << endl;
			cout << vertices[i].Tangent[0] << " " << vertices[i].Tangent[1] << " " << vertices[i].Tangent[2] << endl;
			cout << vertices[i].Bitangent[0] << " " << vertices[i].Bitangent[1] << " " << vertices[i].Bitangent[2] << endl;
		}*/

		for (unsigned long long i = 0; i < vertices.size(); i += 3) {
			glm::vec3 pos1 = vertices[i].Position;
			glm::vec3 pos2 = vertices[i + 1].Position;
			glm::vec3 pos3 = vertices[i + 2].Position;
			glm::vec2 tex1 = vertices[i].TexCoords;
			glm::vec2 tex2 = vertices[i + 1].TexCoords;
			glm::vec2 tex3 = vertices[i + 2].TexCoords;

			glm::vec3 edge1 = pos2 - pos1;
			glm::vec3 edge2 = pos3 - pos1;
			glm::vec2 deltaUV1 = tex2 - tex1;
			glm::vec2 deltaUV2 = tex3 - tex1;

			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			glm::vec3 tangent;
			tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent = glm::normalize(tangent);

			glm::vec3 bitangent;
			bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
			bitangent = glm::normalize(bitangent);

			vertices[i].Tangent = tangent;
			vertices[i + 1].Tangent = tangent;
			vertices[i + 2].Tangent = tangent;
			vertices[i].Bitangent = bitangent;
			vertices[i + 1].Bitangent = bitangent;
			vertices[i + 2].Bitangent = bitangent;
		}
	}
	catch (const std::exception&) {
		cout << "Error: Obj file cannot be read\n";
	}
}

int getModel(vector<Vertex>& vs, string model) {
	// Reset vs:
	vs.clear();

	// Load to vs
	load_obj("./models/" + model, vs);

	// Generate vertices for VBO:
	int size = vs.size() * 14;
	GLfloat* vertices = new GLfloat[size]; // 8 is the size of each vertex obj.
	int k = 0;
	for (int i = 0; i < vs.size(); ++i) {
		vertices[k++] = vs[i].Position[0];
		vertices[k++] = vs[i].Position[1];
		vertices[k++] = vs[i].Position[2];
		vertices[k++] = vs[i].Normal[0];
		vertices[k++] = vs[i].Normal[1];
		vertices[k++] = vs[i].Normal[2];
		vertices[k++] = vs[i].TexCoords[0];
		vertices[k++] = vs[i].TexCoords[1];
		vertices[k++] = vs[i].Tangent[0];
		vertices[k++] = vs[i].Tangent[1];
		vertices[k++] = vs[i].Tangent[2];
		vertices[k++] = vs[i].Bitangent[0];
		vertices[k++] = vs[i].Bitangent[1];
		vertices[k++] = vs[i].Bitangent[2];
	}

	glGenVertexArrays(1, &VAO);  // 专门用来生成VAO的函数， 1 代表生成几个VAO
	glGenBuffers(1, &VBO);   // 专门用来生成VBO的函数

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	// 绑定
	glBindVertexArray(VAO);    // 将VAO与OPENGL绑定
	glBindBuffer(GL_ARRAY_BUFFER, VBO);  // 将VBO与OPENGL绑定，GL_ARRAY_BUFFER是缓冲的类型

	// 向 VBO 中存入vertex数据，第一个参数是类型，第二个是占用多少，第三个是内容（vertexes），最后一个是个参数，
	glBufferData(GL_ARRAY_BUFFER, 4 * size, vertices, GL_STATIC_DRAW);  // GL_STATIC_DRAW代表分配固定的显存，GL_DYNAMIC_DRAW代表显存位置可变，GL_STREAM_DRAW，代表显存总是在变。

	// 告诉VAO内存数据在VAO中是如何储存的，VAO中从0开始，VBO中三个GL_FLOAT一组，6 * sizeof(GLfloat)为下一个pos坐标在VBO中的位置。
	// Position:
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normal:
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Texture:
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// Tangent:
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	// Bitangent:
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));
	glEnableVertexAttribArray(4);

	glBindVertexArray(0); // Unbind VAO

	return size / 14;
}


void setCallbacks(GLFWwindow* window) {
	glfwSetCursorPosCallback(window,
		[](GLFWwindow*, double x, double y) {
			screen->cursorPosCallbackEvent(x, y);
		}
	);

	glfwSetMouseButtonCallback(window,
		[](GLFWwindow*, int button, int action, int modifiers) {
			screen->mouseButtonCallbackEvent(button, action, modifiers);
		}
	);

	glfwSetKeyCallback(window,
		[](GLFWwindow*, int key, int scancode, int action, int mods) {
			screen->keyCallbackEvent(key, scancode, action, mods);
		}
	);

	glfwSetCharCallback(window,
		[](GLFWwindow*, unsigned int codepoint) {
			screen->charCallbackEvent(codepoint);
		}
	);

	glfwSetDropCallback(window,
		[](GLFWwindow*, int count, const char** filenames) {
			screen->dropCallbackEvent(count, filenames);
		}
	);

	glfwSetScrollCallback(window,
		[](GLFWwindow*, double x, double y) {
			screen->scrollCallbackEvent(x, y);
		}
	);

	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow*, int width, int height) {
			screen->resizeCallbackEvent(width, height);
		}
	);
}

void initGui() {
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);     // 大版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);		// 小版本号
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 设置工作模式为核心模式，不兼容管线。
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
}