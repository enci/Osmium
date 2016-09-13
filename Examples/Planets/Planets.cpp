#include "stdafx.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Core/Device.h>
#include <Core/World.h>
#include <Graphics/DebugRenderer.h>
#include <Core/Transform.h>
#include "Gameplay/PlanetWorld.h"

using namespace Osm;

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

	window = glfwCreateWindow(1280, 720, "GLSL workshop", nullptr, nullptr);

	int major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
	int minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
	int revision = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
	cout << "OpenGL Version " << major << "." << minor << "." << revision << endl;

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
		cout << "Failed to initialize OpenGL context" << endl;
		return -1;
	}
	InitDebugMessages();

	float ratio;
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / float(height);

	auto lastFrame = glfwGetTime();

	pDevice = new Device(window);
	World* world = new PlanetWorld();

	while (!glfwWindowShouldClose(window))
	{
		auto currentFrame = glfwGetTime();
		auto deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glViewport(0, 0, width, height);
		Osm::Color bg("000a38");
		glClearColor(bg.r / 255.0f, bg.g / 255.0f, bg.b / 255.0f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		
		world->Update(deltaTime);
		world->Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete world;
	delete pDevice;

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}

