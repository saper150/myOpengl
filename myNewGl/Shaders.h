#pragma once
#include "stdafx.h"
#include <map>

struct Shaders{
private:
	Shaders();

	std::unordered_map<int, std::string> fragShaders;
	std::unordered_map<int, std::string> vertexShaders;
	std::map<std::pair<int, int>, GLuint> compiledPrograms;

	GLuint compileShader(const std::string& code, GLenum type);
	GLuint compileProgram(GLuint vertex, GLuint fragment);

	static Shaders instance;

public:
	static Shaders& getInstance() {
		return instance;
	}

	GLuint getProgram(int vetrexCode, int fragmentCode);

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