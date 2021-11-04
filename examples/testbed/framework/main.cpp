/*
* Copyright (c) 2016-2019 Irlan Robson 
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "model.h"
#include "view.h"
#include "view_model.h"

#include <stdio.h>

extern void DrawString(const b3Color& color, const char* string, ...);

GLFWwindow* g_window;

static Model* g_model;
static View* g_view;
static ViewModel* g_viewModel;

static void glfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "GLFW error occured. Code: %d. Description: %s\n", error, description);
}

static void WindowSize(GLFWwindow* ww, int w, int h)
{
	g_view->Event_SetWindowSize(w, h);
}

static void CursorMove(GLFWwindow* w, double x, double y)
{
	g_view->Event_Move_Cursor(float(x), float(y));
}

static void WheelScroll(GLFWwindow* w, double dx, double dy)
{
	g_view->Event_Scroll(float(dx), float(dy));
}

static void MouseButton(GLFWwindow* w, int button, int action, int mods)
{
	switch (action)
	{
	case GLFW_PRESS:
	{
		g_view->Event_Press_Mouse(button);
		break;
	}
	case GLFW_RELEASE:
	{
		g_view->Event_Release_Mouse(button);
		break;
	}
	default:
	{
		break;
	}
	}
}

static void KeyButton(GLFWwindow* w, int button, int scancode, int action, int mods)
{
	switch (action)
	{
	case GLFW_PRESS:
	{
		g_view->Event_Press_Key(button);
		break;
	}
	case GLFW_RELEASE:
	{
		g_view->Event_Release_Key(button);
		break;
	}
	default:
	{
		break;
	}
	}
}

static void Run()
{
	int w, h;
	glfwGetWindowSize(g_window, &w, &h);
	g_view->Event_SetWindowSize(u32(w), u32(h));

	while (glfwWindowShouldClose(g_window) == 0)
	{
		g_profiler->Begin();

		g_profiler->OpenScope("Frame");

		g_view->BeginInterface();

		if (g_model->IsPaused())
		{
			DrawString(b3Color_white, "*PAUSED*");
		}
		else
		{
			DrawString(b3Color_white, "*PLAYING*");
		}

		g_model->Update();

		g_profiler->CloseScope();
		
		g_view->Interface();

		g_view->RenderInterface();

		g_profiler->End();

		glfwSwapBuffers(g_window);
		glfwPollEvents();
	}
}

int main(int argc, char** args)
{
#if defined(_WIN32)
	// Report memory leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
	//_CrtSetBreakAlloc();
#endif

	glfwSetErrorCallback(glfwErrorCallback);

	if (glfwInit() == 0)
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

#if __APPLE__
	const char* glslVersion = "#version 150";
#else
	const char* glslVersion = NULL;
#endif

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window
	char buffer[128];
	sprintf_s(buffer, "Bounce Testbed Version %d.%d.%d", b3_version.major, b3_version.minor, b3_version.revision);

	bool fullscreen = false;
	if (fullscreen)
	{
		g_window = glfwCreateWindow(1920, 1080, buffer, glfwGetPrimaryMonitor(), NULL);
	}
	else
	{
		g_window = glfwCreateWindow(1280, 720, buffer, NULL, NULL);
	}

	if (g_window == NULL)
	{
		fprintf(stderr, "Failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(g_window);

	// Load OpenGL functions using glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Failed to load OpenGL functions using glad\n");
		glfwTerminate();
		return -1;
	}

	printf("GL %d.%d\n", GLVersion.major, GLVersion.minor);
	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

	glfwSetWindowSizeCallback(g_window, WindowSize);
	glfwSetCursorPosCallback(g_window, CursorMove);
	glfwSetScrollCallback(g_window, WheelScroll);
	glfwSetMouseButtonCallback(g_window, MouseButton);
	glfwSetKeyCallback(g_window, KeyButton);
	glfwSwapInterval(1);

	g_model = new Model();
	g_view = new View(g_window, glslVersion);
	g_viewModel = new ViewModel(g_model, g_view);

	// Run
	Run();

	delete g_viewModel;
	g_viewModel = nullptr;

	delete g_view;
	g_view = nullptr;

	delete g_model;
	g_model = nullptr;

	glfwTerminate();
	g_window = nullptr;

	return 0;
}
