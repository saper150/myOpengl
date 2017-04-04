#include "stdafx.h"
#include "Shaders.h"

Shaders::Shaders() {

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
		"uniform mat4 MVP;"
		"uniform float spiralPosition;"
		"uniform float totalSize;"
		"out vec4 fragColor;"
		"void main(){"
		"float p = position.y+(totalSize*spiralPosition);"
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

	{
		const  char * vertexCode =
			"#version 330 core\n"
			"layout(location = 0) in vec3 pointPosition;"
			"layout(location = 1) in vec3 normal;"
			"layout(location = 2) in vec2 vertexUV;"
			"uniform mat4 model;"
			"uniform mat4 view;"
			"uniform mat4 projection;"
			"out vec2 UV;"
			"out vec3 normalFrag;"
			"out vec3 fragPos;"
			"void main(){"
			"UV = vertexUV;"
			"normalFrag = normal;"
			"fragPos = vec3(model*vec4(pointPosition,1.0));"
			"gl_Position = projection*view*model*vec4(pointPosition,1.0);"
			"}";

		const char* fragCode =
			"#version 330 core\n"
			"out vec4 color;"
			"in vec2 UV;"
			"in vec3 normalFrag;"
			"in vec3 fragPos;"
			"uniform sampler2D textureSampler;"
			"uniform float ambientLightStrength;"
			"uniform vec3 ambientLightColor;"
			"void main(){"
			"vec3 lightDir = normalize(vec3(0,0,0)-fragPos);"
			"vec3 diff = max(dot(normalFrag,lightDir),0.0)*vec3(1.0,1.0,1.0);"
			"vec3 ambient = (ambientLightStrength*ambientLightColor);"
			"color =  vec4(ambient+diff,1.0) * texture(textureSampler,vec2(UV.x,1.0-UV.y));"
				"}";

			const auto vertex = compileShader(vertexCode, GL_VERTEX_SHADER);
			const auto frag = compileShader(fragCode, GL_FRAGMENT_SHADER);
			auto program = compileProgram(vertex,frag);

			std::unordered_map<std::string, GLuint> uniformLocations;
			uniformLocations["textureSampler"] = glGetUniformLocation(program, "textureSampler");
			uniformLocations["ambientLightStrength"] = glGetUniformLocation(program, "ambientLightStrength");
			uniformLocations["ambientLightColor"] = glGetUniformLocation(program, "ambientLightColor");
			uniformLocations["model"] = glGetUniformLocation(program, "model");
			uniformLocations["view"] = glGetUniformLocation(program, "view");
			uniformLocations["projection"] = glGetUniformLocation(program, "projection");

			programs[LIGTH_PROGRAM] = std::make_unique<ProgramDescription>(ProgramDescription(program, uniformLocations));
	}
}

GLuint Shaders::compileShader(const std::string& code, GLenum type) {
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

GLuint Shaders::compileProgram(GLuint vertex, GLuint fragment) {

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

GLuint Shaders::getProgram(int vetrexCode, int fragmentCode) {

	auto got = compiledPrograms.find(std::make_pair(vetrexCode, fragmentCode));
	if (got != compiledPrograms.end()) {
		return got->second;
	}

	std::cout << "compiling vertex:" << vetrexCode << " fragment:" << fragmentCode << std::endl;

	auto vertex = compileShader(vertexShaders[vetrexCode], GL_VERTEX_SHADER);
	auto fragment = compileShader(fragShaders[fragmentCode], GL_FRAGMENT_SHADER);

	GLuint programId = compileProgram(vertex, fragment);
	compiledPrograms.insert(std::make_pair(std::make_pair(vetrexCode, fragmentCode), programId));

	return programId;

}

ProgramDescription * Shaders::getProgram(PROGRAMS programId)
{
	auto f = programs.find(programId);
	if (f == programs.end()) {
		throw std::runtime_error("program not found");
	}
	return (*f).second.get();
}
