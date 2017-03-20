#pragma once
#include "stdafx.h"
#include <map>

struct Shaders{
private:
	Shaders()
	{
		fragShaders[IN_OUT_COLOR] =
			"#version 330 core\n"
			"out vec4 color;"
			"in vec4 fragColor;"
			"void main(){"
				"color = fragColor;"
			"}";
		fragShaders[FLAT_COLOR] =
			"#version 330 core\n"
			"out vec4 color;"
			"flat in vec4 fragColor;"
			"void main(){"
			"color = fragColor;"
			"}";


		fragShaders[TEXTURE] =
			"#version 330 core\n"
			"out vec4 color;"
			"in vec2 UV;"
			"uniform sampler2D textureSampler;"
			"void main(){"
			"color = texture(textureSampler,vec2(UV.x,1.0-UV.y));"
			"}";

		fragShaders[FRAG_SKYBOX] =
			"#version 330 core\n"
			"in vec3 direction;"
			"out vec4 color;"
			"uniform samplerCube cubeSampler;"
			"void main(){"
			"color = texture(cubeSampler,direction);"
			"}";



		vertexShaders[POINT] =
			"#version 330 core\n"
			"layout(location = 0) in vec3 position;"
			"layout(location = 1) in vec3 color;"
			"layout(location = 2) in float size;"
			"uniform mat4 MVP;"
			"out vec4 fragColor;"
			"void main(){"
			"fragColor = vec4(color,1.0);"
			"gl_Position = MVP*vec4(position,1.0);"
			"gl_PointSize = size;"
			"}";

		vertexShaders[FLAT_POINT] =
			"#version 330 core\n"
			"layout(location = 0) in vec3 position;"
			"layout(location = 1) in vec3 color;"
			"layout(location = 2) in float size;"
			"uniform mat4 MVP;"
			"flat out vec4 fragColor;"
			"void main(){"
			"fragColor = vec4(color,1.0);"
			"gl_Position = MVP*vec4(position,1.0);"
			"gl_PointSize = size;"
			"}";

		vertexShaders[SPIRAL] =
			"#version 330 core\n"
			"layout(location = 0) in vec3 position;"
			"layout(location = 1) in vec3 color;"
			"layout(location = 2) in float size;"
			"uniform mat4 MVP;"
			"uniform float spiralPosition;"
			"uniform float totalSize;"
			"float p;"
			"float transparency;"
			"out vec4 fragColor;"
			"void main(){"
			"p = position.y+(totalSize*spiralPosition);"
			"if(p>totalSize){"
			"p = p - totalSize;"
			"}"
			"gl_Position = MVP*vec4(position.x,p,position.z,1.0);"
			"float a = p/totalSize;"

			"fragColor = vec4(color,a);"
			"gl_PointSize = a*10.0;"
			"}";

		vertexShaders[UV] =
			"#version 330 core\n"
			"layout(location = 0) in vec3 pointPosition;"
			"layout(location = 1) in vec2 vertexUV;"
			"uniform mat4 MVP;"
			"out vec2 UV;"
			"void main(){"
			"UV = vertexUV;"
			"gl_Position = MVP*vec4(pointPosition,1.0);"
			"}";

		vertexShaders[SKYBOX] =
			"#version 330 core\n"
			"layout(location = 0) in vec3 position;"
			"uniform mat4 MVP;"
			"out vec3 direction;"
			"void main(){"
			"direction = position;"
			"gl_Position = MVP*vec4(position,1.0);"
			"}";
	}

	std::unordered_map<int, std::string> fragShaders;
	std::unordered_map<int, std::string> vertexShaders;
	std::map<std::pair<int, int>, GLuint> compiledPrograms;


	GLuint compileShader(const std::string& code, GLenum type) {
		GLuint shader = glCreateShader(type);
		const char* c = code.c_str();
		glShaderSource(shader, 1, &c, NULL);
		glCompileShader(shader);

		GLint result;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		int logInfoLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logInfoLength);
		if (logInfoLength > 0) {
			std::vector<char> errorMsg(logInfoLength + 1);
			glGetShaderInfoLog(shader, logInfoLength, NULL, errorMsg.data());
			std::cout << errorMsg.data() << std::endl;
		}
		return shader;
	}

	GLuint compileProgram(GLuint vertex, GLuint fragment) {

		GLuint program = glCreateProgram();
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);

		GLint result;
		glGetProgramiv(program, GL_LINK_STATUS, &result);
		int logInfoLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logInfoLength);
		if (logInfoLength > 0) {
			std::vector<char> errorMsg(logInfoLength + 1);
			glGetShaderInfoLog(program, logInfoLength, NULL, errorMsg.data());
			std::cout << errorMsg.data() << std::endl;
		}

		glDetachShader(program, vertex);
		glDetachShader(program, fragment);

		glDeleteShader(vertex);
		glDeleteShader(fragment);

		return program;

	}
	static Shaders instance;

public:
	static Shaders& getInstance() {
		return instance;
	}

	template<typename T>
	T getProgram(int vetrexCode, int fragmentCode) {

		auto got = compiledPrograms.find(std::make_pair(vetrexCode, fragmentCode));
		if (got != compiledPrograms.end()) {
			return T(got->second);
		}

		std::cout << "compiling vertex:" << vetrexCode << " fragment:" << fragmentCode << std::endl;

		auto vertex = compileShader(vertexShaders[vetrexCode], GL_VERTEX_SHADER);
		auto fragment = compileShader(fragShaders[fragmentCode], GL_FRAGMENT_SHADER);

		GLuint programId = compileProgram(vertex, fragment);
		compiledPrograms.insert(std::make_pair(std::make_pair(vetrexCode, fragmentCode), programId));

		return T(programId);

	}

	enum fragment {
		IN_OUT_COLOR = 0,
		TEXTURE = 1,
		FLAT_COLOR = 2,
		FRAG_SKYBOX = 3,

	};

	enum vertex {
		POINT = 0,
		UV = 1,
		SKYBOX = 2,
		SPIRAL = 3,
		FLAT_POINT = 4
	};

};