#pragma once
#include "stdafx.h"
#include <map>


struct ProgramDescription {

	ProgramDescription(const GLuint program,const std::unordered_map<std::string, GLuint> uniformLocations)
		:program(program), uniformLocations(uniformLocations)
	{
			
	}
	const GLuint program;
	const std::unordered_map<std::string, GLuint> uniformLocations;
};


enum PROGRAMS {
	LIGTH_PROGRAM

};


struct Shaders{
private:
	Shaders();

	std::unordered_map<int, std::string> fragShaders;
	std::unordered_map<int, std::string> vertexShaders;
	std::map<std::pair<int, int>, GLuint> compiledPrograms;

	GLuint compileShader(const std::string& code, GLenum type);
	GLuint compileProgram(GLuint vertex, GLuint fragment);

	std::unordered_map<PROGRAMS, std::unique_ptr<ProgramDescription>> programs;

public:
	static Shaders& getInstance() {
		static Shaders instance;

		return instance;
	}

	GLuint getProgram(int vetrexCode, int fragmentCode);
	ProgramDescription * getProgram(PROGRAMS programId);

	enum fragment {
		IN_OUT_COLOR = 0,
		TEXTURE = 1,
		FLAT_COLOR = 2,
		FRAG_SKYBOX = 3,
		LIGHT_FRAG = 4
	};

	enum vertex {
		POINT = 0,
		UV = 1,
		SKYBOX = 2,
		SPIRAL = 3,
		FLAT_POINT = 4,
		LIGHT_VERTEX = 5
	};

};