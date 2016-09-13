#include "stdafx.h"
//#include <vld.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#include "Asteroids.h"

#include <iostream>
#include <Core/Device.h>
#include "Gameplay/Blasteroids.h"
#include "UI/ScoreScreen.h"

using namespace igad;

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(void)
{
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwSetErrorCallback(error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);		 // yes, 3 and 2!!!
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // But also 4 if present
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	GLFWwindow* window;
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	//window = glfwCreateWindow(mode->width, mode->height, "GLSL workshop", glfwGetPrimaryMonitor(), nullptr);
	window = glfwCreateWindow(1920, 1080, "GLSL workshop", nullptr, nullptr);
	// window = glfwCreateWindow(640, 360, "Blasteroids", nullptr, nullptr);

	int major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
	int minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
	int revision = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
	std::cout << "OpenGL Version " << major << "." << minor << "." << revision << std::endl;

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}
	InitDebugMessages();

	float ratio;
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / float(height);

	auto lastFrame = glfwGetTime();

	pDevice = new Device(window);
	igad::World* world = nullptr;
	float timer = 2.0f;
	int worldkind = 0;

	while (!glfwWindowShouldClose(window))
	{
		if (world == nullptr)
		{
			if (worldkind % 2 == 0)
				world = new Blasteroids();
			else
				world = new ScoreScreen();

			worldkind++;
		}

		auto currentFrame = glfwGetTime();
		auto deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glViewport(0, 0, width, height);

		if (deltaTime > 0.033f)
			deltaTime = 0.033f;

		world->Update(deltaTime);
		world->Render();


		if (world->IsDone())
		{
			if (timer > 0)
			{
				timer -= deltaTime;
			}
			else
			{
				timer = 2.0f;
				delete world;
				world = nullptr;
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete world;
	delete pDevice;

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}

