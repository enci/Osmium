#pragma once

#include <glad/glad.h>
#include <Defines.h>
#include <iostream>
#include <sstream>

#define OPENGL_ES 0

namespace Osm
{
#ifdef DEBUG

#if OPENGL_ES

// Convert a GLenum to a readable c-string
inline const char* GLErrorString(GLenum error)
{
	switch (error)
	{
	case GL_INVALID_ENUM:
		return "GL_INVALID_ENUM";

	case GL_INVALID_VALUE:
		return "GL_INVALID_VALUE";

	case GL_INVALID_OPERATION:
		return "GL_INVALID_OPERATION";

	case GL_OUT_OF_MEMORY:
		return "GL_OUT_OF_MEMORY";

	default:
		return "UNKNOWN";
	}
}

#define GL_GET_ERROR()                                              \
{                                                                   \
    GLenum error;                                                   \
    bool err = false;                                               \
    while ( (error = glGetError()) != GL_NO_ERROR)                  \
    {                                                               \
        LOG( "OpenGL ERROR: %s\nCHECK POINT: %s (line %d)\n",       \
        Osm::GLErrorString(error), __FILE__, __LINE__ );			\
        err = true;                                                 \
    }                                                               \
    ASSERT(!err);                                                   \
}

#define GL_CLEAR_ERROR()                                            \
{                                                                   \
    GLenum error;                                                   \
    while ( (error = glGetError()) != GL_NO_ERROR);                 \
}

#else

static std::string FormatDebugOutput(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	const char* msg)
{
	std::stringstream stringStream;
	std::string sourceString;
	std::string typeString;
	std::string severityString;

	// The AMD variant of this extension provides a less detailed classification of the error,
	// which is why some arguments might be "Unknown".
	switch (source) {
		case GL_DEBUG_CATEGORY_API_ERROR_AMD:
		case GL_DEBUG_SOURCE_API: {
			sourceString = "API";
			break;
		}
		case GL_DEBUG_CATEGORY_APPLICATION_AMD:
		case GL_DEBUG_SOURCE_APPLICATION: {
			sourceString = "Application";
			break;
		}
		case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
			sourceString = "Window System";
			break;
		}
		case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:
		case GL_DEBUG_SOURCE_SHADER_COMPILER: {
			sourceString = "Shader Compiler";
			break;
		}
		case GL_DEBUG_SOURCE_THIRD_PARTY: {
			sourceString = "Third Party";
			break;
		}
		case GL_DEBUG_CATEGORY_OTHER_AMD:
		case GL_DEBUG_SOURCE_OTHER: {
			sourceString = "Other";
			break;
		}
		default: {
			sourceString = "Unknown";
			break;
		}
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR: {
		typeString = "Error";
		break;
	}
	case GL_DEBUG_CATEGORY_DEPRECATION_AMD:
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
		typeString = "Deprecated Behavior";
		break;
	}
	case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
		typeString = "Undefined Behavior";
		break;
	}
	case GL_DEBUG_TYPE_PORTABILITY_ARB: {
		typeString = "Portability";
		break;
	}
	case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:
	case GL_DEBUG_TYPE_PERFORMANCE: {
		typeString = "Performance";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_TYPE_OTHER: {
		typeString = "Other";
		break;
	}
	default: {
		typeString = "Unknown";
		break;
	}
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: {
		severityString = "High";
		break;
	}
	case GL_DEBUG_SEVERITY_MEDIUM: {
		severityString = "Medium";
		break;
	}
	case GL_DEBUG_SEVERITY_LOW: {
		severityString = "Low";
		break;
	}
	default: {
		severityString = "Unknown";
		break;
	}
	}

	stringStream << "OpenGL Error: " << msg;
	stringStream << " [Source = " << sourceString;
	stringStream << ", Type = " << typeString;
	stringStream << ", Severity = " << severityString;
	stringStream << ", ID = " << id << "]";

	return stringStream.str();
}

static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam)
{
	switch(id)
	{
	case 131218:	// http://stackoverflow.com/questions/12004396/opengl-debug-context-performance-warning
		return;
	}

	std::string error = FormatDebugOutput(source, type, id, severity, message);
	std::cout << error << std::endl;
}

static void InitDebugMessages()
{
	// Query the OpenGL function to register your callback function.
	PFNGLDEBUGMESSAGECALLBACKPROC _glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)wglGetProcAddress("glDebugMessageCallback");
	PFNGLDEBUGMESSAGECALLBACKARBPROC _glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC)wglGetProcAddress("glDebugMessageCallbackARB");
	PFNGLDEBUGMESSAGECALLBACKAMDPROC _glDebugMessageCallbackAMD = (PFNGLDEBUGMESSAGECALLBACKAMDPROC)wglGetProcAddress("glDebugMessageCallbackAMD");

	glDebugMessageCallback(DebugCallback, nullptr);

	// Register your callback function.
	if (_glDebugMessageCallback != nullptr) {
		_glDebugMessageCallback(DebugCallback, nullptr);
	}
	else if (_glDebugMessageCallbackARB != nullptr) {
		_glDebugMessageCallbackARB(DebugCallback, nullptr);
	}
	/*
	// No AMD support
	else if (_glDebugMessageCallbackAMD != NULL)
	{
		_glDebugMessageCallbackAMD(DebugCallback, NULL);
	}
	*/

	// Enable synchronous callback. This ensures that your callback function is called
	// right after an error has occurred. This capability is not defined in the AMD
	// version.
	if ((_glDebugMessageCallback != nullptr) || (_glDebugMessageCallbackARB != nullptr))
	{
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}

	glDebugMessageControl(
		GL_DONT_CARE,
		GL_DONT_CARE,
		GL_DEBUG_SEVERITY_LOW,
		0,
		nullptr,
		GL_FALSE);

	glDebugMessageControl(
		GL_DONT_CARE,
		GL_DONT_CARE,
		GL_DEBUG_SEVERITY_NOTIFICATION,
		0,
		nullptr,
		GL_FALSE);
}

// No need for this when not running ES
#define GL_CLEAR_ERROR()
#define GL_GET_ERROR()
#endif

#else
#define InitDebugMessages()
#define GL_CLEAR_ERROR()
#define GL_GET_ERROR()
#endif

#define DEBUG

} // Osm
