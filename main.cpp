#include <stdio.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <math.h>
#include <vector>

#include "Block.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Dependencies/include/rapidxml-1.13/rapidxml.hpp"
#include "Dependencies/include/rapidxml-1.13/rapidxml_print.hpp"
#include "Dependencies/include/rapidxml-1.13/rapidxml_utils.hpp"
#include "./Dependencies/include/ImGuiFileDialog-Lib_Only/ImGuiFileDialog.h"

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720
#define EPS 0.1
#define PRECISION 1.0f
using namespace rapidxml;
using namespace std;

glm::vec3 cameraPos, cameraFront, cameraUp, lookAt, moving_up;
unsigned int width_, height_;

int e = 0;
glm::mat4 projection, view, model, mvp;
glm::mat4 projection_i, view_i, model_i, mvp_i;
glm::vec2 mousePosOld, angle;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float ipd = 0.01f;
float d = 0.1f;
float near = 0.001f;
float far = 200.0f;
bool animate = false;
float T = 0.0f;
float animation_time = 1.0f;
bool show_frames = false;
int number_of_frames_shown = 15;

float distance_d = 1.0f;
float speed = 0.2f; // speed of milling tool in milimeters per second;
float size_x = 1;
float size_y = 1;
float size_z = 1;
int divisions_x = 400;
int divisions_y = 400;
glm::vec3 translation_s;
glm::vec3 translation_e;
glm::vec3 rot_euler_s;
glm::vec3 rot_euler_e;
glm::quat quaternion_s = { 1,0,0,0 };
glm::quat quaternion_e = { 1,0,0,0 };
bool is_linear_aprox = true;

Camera cam;

glm::vec3 start_poss;
glm::vec3 end_poss;

glm::quat start_q;
glm::quat end_q;

std::unique_ptr<Cursor> end_pos;
std::unique_ptr<Cursor> end_pos2;
Shader ourShader;
Shader ourShader2;
GLFWwindow* window;
GLFWwindow* window2;
//std::unique_ptr<Cursor> cursor, center;

std::vector<std::shared_ptr<Object>> objects_list = {};
std::unique_ptr<Block> block;
std::unique_ptr<Block> block2;

void draw_scene();
void draw_scene2();
void framebuffer_size_callback(GLFWwindow* window_1, int width, int height);
void framebuffer_size_callback2(GLFWwindow* window_1, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void create_gui();

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "SimpleCAD 1", NULL, NULL);
	window2 = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "SimpleCAD 2", NULL, window);
	if (window == NULL || window2 == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	model = glm::mat4(1.0f);
	view = glm::mat4(1.0f);

	cameraPos = { 0,0,5 };
	cameraFront = { 0,0,-1 };
	lookAt = { 0,0,0 };
	cameraUp = { 0,1,0 };

	angle = { -90.0f, 0.0f };
	width_ = DEFAULT_WIDTH;
	height_ = DEFAULT_HEIGHT;

	cam = Camera(cameraPos, cameraFront, cameraUp);
	cam.SetPerspective(glm::radians(45.0f), DEFAULT_WIDTH / (float)DEFAULT_HEIGHT, near, far);


	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	const char* glsl_version = "#version 330";
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// build and compile our shader program
	// ------------------------------------
	ourShader = Shader("shader.vs", "shader.fs"); // you can name your shader files however you like

	glEnable(GL_DEPTH_TEST);

	block = std::make_unique<Block>(ourShader);
	end_pos = std::make_unique<Cursor>(ourShader);
	glfwMakeContextCurrent(window2);

	glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	glfwSetFramebufferSizeCallback(window2, framebuffer_size_callback2);
	glfwSetCursorPosCallback(window2, mouse_callback);
	glfwSetScrollCallback(window2, scroll_callback);

	// build and compile our shader program
	// ------------------------------------
	ourShader2 = Shader("shader.vs", "shader.fs"); // you can name your shader files however you like
	glEnable(GL_DEPTH_TEST);

	block2 = std::make_unique<Block>(ourShader2);
	end_pos2 = std::make_unique<Cursor>(ourShader2);
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		glfwMakeContextCurrent(window);
		// cleaning frame
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		projection = cam.GetProjectionMatrix();
		projection_i = glm::inverse(projection);
		view = cam.GetViewMatrix();
		view_i = glm::inverse(view);

		mvp = projection * view;

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		create_gui();

			draw_scene();

		// Render dear imgui into screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// check and call events and swap the buffers
		glfwPollEvents();
		glfwSwapBuffers(window);

		glfwMakeContextCurrent(window2);
		glClearColor(0.8f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			draw_scene2();
		glfwSwapBuffers(window2);

		if (animate) {
			if (T < 1.0f) {
				T += deltaTime / animation_time;
			}
		}
	}

	// cleanup stuff
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	objects_list.clear();
	ourShader.deleteShader();
	return 0;
}

void draw_scene() {
	if (animate) {
		block->DrawFrame(T, start_poss, end_poss, start_q, end_q);
	}
	block->DrawObject(mvp);
	end_pos->DrawObject(mvp);
}

void draw_scene2() {
	if (animate) {
		block2->DrawFrame(T);
	}
	block2->DrawObject(mvp);
	end_pos2->DrawObject(mvp);
}

#pragma region  boilerCodeOpenGL

void framebuffer_size_callback(GLFWwindow* window_1, int width, int height)
{
	glfwMakeContextCurrent(window_1);
	glViewport(0, 0, width, height);


	glfwMakeContextCurrent(window2);
	glViewport(0, 0, width, height);
	glfwSetWindowSize(window2, width, height);

	cam.SetPerspective(glm::radians(45.0f), width / (float)height, near, far);
	width_ = width;
	height_ = height;
}

void framebuffer_size_callback2(GLFWwindow* window_2, int width, int height)
{
	glfwMakeContextCurrent(window);
	glViewport(0, 0, width, height);
	glfwSetWindowSize(window, width, height);

	glfwMakeContextCurrent(window_2);
	glViewport(0, 0, width, height);

	cam.SetPerspective(glm::radians(45.0f), width / (float)height, near, far);
	width_ = width;
	height_ = height;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;
	glm::vec2 mousePos = { xpos,ypos };
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		glm::vec2 diff = (mousePosOld - mousePos) * PRECISION;
		float cameraSpeed = 30 * deltaTime;
		float radius;
		diff *= cameraSpeed;

		glm::vec3 right_movement = cam.GetRightVector() * -diff.x;
		glm::vec3 up_movement = cam.GetUpVector() * diff.y;
		glm::vec3 angle2 = lookAt - (cameraPos + right_movement + up_movement);

		auto rotation = Object::RotationBetweenVectors(lookAt - cameraPos, angle2);
		auto roation = glm::toMat4(rotation);
		angle += diff;
		if (angle.y > 90.0f) angle.y = 90.0f - EPS;
		if (angle.y < -90.0f) angle.y = -90.0f + EPS;
		if (angle.x > 180.0f) angle.x = -180.0f + EPS;
		if (angle.x < -180.0f) angle.x = 180.0f - EPS;
		radius = glm::length(cameraPos - lookAt);

		cameraPos.x = lookAt.x + radius * glm::cos(glm::radians(angle.y)) * glm::cos(glm::radians(angle.x));
		cameraPos.z = lookAt.z + radius * -glm::cos(glm::radians(angle.y)) * glm::sin(glm::radians(angle.x));
		cameraPos.y = lookAt.y + radius * glm::sin(glm::radians(-angle.y));

		cameraFront = glm::normalize(lookAt - cameraPos);
		cam.LookAt(cameraPos, cameraFront, cameraUp);
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		glm::vec2 diff = (mousePosOld - mousePos) * PRECISION;
		float cameraSpeed = speed * deltaTime;

		glm::vec2 movement = diff * cameraSpeed;

		glm::vec3 right_movement = cam.GetRightVector() * movement.x;
		glm::vec3 up_movement = cam.GetUpVector() * -movement.y;
		cameraPos += right_movement + up_movement;
		lookAt += right_movement + up_movement;

		cam.LookAt(cameraPos, cameraFront, cameraUp);
	}
	mousePosOld = mousePos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;

	float precision = 0.01f;

	float movement = 1.0f - yoffset * precision;
	if (movement <= 0.0f)
		movement = 0.1f;
	cameraFront = glm::normalize(lookAt - cameraPos);
	float dist = glm::length(lookAt - cameraPos);
	cameraPos = lookAt - (cameraFront * dist * movement);
	cam.LookAt(cameraPos, cameraFront, cameraUp);

}

#pragma endregion

void create_gui() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow();
	bool open;
	ImGuiWindowFlags flags = 0;
	//flags |= ImGuiWindowFlags_MenuBar;
	ImGui::Begin("Main Menu##uu", &open, flags);

	//ImGui::InputFloat3("Start Position", (float*)&translation_s);
	ImGui::InputFloat3("End Position", (float*)&translation_e);

	//ImGui::InputFloat4("Quaternion start", (float*)&quaternion_s);
	ImGui::InputFloat4("Quaternion end", (float*)&quaternion_e);

	ImGui::SliderFloat("Animation time", &animation_time, 0.1f, 100.0f, "%.3f", 1.0f);
	ImGui::Checkbox("Linear aproximation (true) or spherical (false)", &is_linear_aprox);

	ImGui::Checkbox("Show frame by frame", &show_frames);
	if (show_frames)
		ImGui::SliderInt("Number of frames shown", &number_of_frames_shown, 1, 200);

	if (ImGui::Button("position")) {
		end_pos->SetCursorPosition(translation_e);
		end_pos->RotateObject(quaternion_e);
		block->SolveInverse(translation_e, quaternion_e);
		block2->SolveInverse(translation_e, quaternion_e);
	}
	if (ImGui::Button("Set End position")) {
		block->end = block->current;
		block2->end = block2->current;
		end_poss = translation_e;
		end_q = quaternion_e;
	}

	if (ImGui::Button("Set Start position")) {
		block->start = block->current;
		block2->start = block2->current;
		start_poss = translation_e;
		start_q = quaternion_e;
	}

	if (ImGui::Button("Choose first")) {
		block->current = block->first;
		block2->current = block2->first;
	}
	if (ImGui::Button("Choose second")) {
		block->current = block->second;
		block2->current = block2->second;
	}

	
	if (ImGui::Button("Normalize quaternion")) {
		quaternion_s = glm::normalize(quaternion_s);
		quaternion_e = glm::normalize(quaternion_e);

		end_pos->SetCursorPosition(translation_e);
		end_pos->RotateObject(quaternion_e);
		end_pos2->SetCursorPosition(translation_e);
		end_pos2->RotateObject(quaternion_e);
	}

	if (ImGui::Button("Start Animation")) animate = true;
	if (ImGui::Button("Stop Animation")) animate = false;
	if (ImGui::Button("Restart Animation")) T = 0.0f;

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}
