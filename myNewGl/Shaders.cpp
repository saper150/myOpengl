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
		"float p = position.y+(totalSize*2*spiralPosition);"
		"if(p>totalSize){"
		"p = p - totalSize*2;"
		"}"
		"gl_Position = MVP*vec4(position.x,p,position.z,1.0);"
		"float a = p/totalSize*2;"
		"float size = ((a+1)/2)*(9.0)+1.0;"
		"float b = a+1.0;"
		"fragColor = vec4(color,size/10.0);"
		"gl_PointSize = size;"
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
	//LIGTH_PROGRAM
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
			"normalFrag = (model*vec4(normal,0.0)).xyz;"
			"fragPos = vec3(model*vec4(pointPosition,1.0));"
			"gl_Position = projection*view*model*vec4(pointPosition,1.0);"
			"}";

		const char* fragCode =
			"#version 330 core\n"
			"out vec4 color;"
			"in vec2 UV;"
			"in vec3 normalFrag;"
			"in vec3 fragPos;"
			"\n"
			"#define MAX_LIGHT 5\n"

			"struct Light{"
			"vec3 position[MAX_LIGHT];"
			"vec3 ambient[MAX_LIGHT];"
			"vec3 diffuse[MAX_LIGHT];"
			"vec3 specular[MAX_LIGHT];"
			"float constant[MAX_LIGHT];"
			"float linear[MAX_LIGHT];"
			"float quadratic[MAX_LIGHT];"
			"};"

			"uniform vec3 viewPos;"
			"uniform Light light;"
			"uniform int lightCount;"
			"uniform sampler2D textureSampler;"

			"void main(){"

			"vec3 res = vec3(0,0,0);"
			"for(int i = 0;i<lightCount;i++){"
			"float distance = length(light.position[i] - fragPos);"
			"float attenuation = 1.0/(light.constant[i]+light.linear[i]*distance"
			"+ light.quadratic[i]*(distance*distance));"
			"vec4 colorr  = texture(textureSampler,vec2(UV.x,1.0-UV.y));"
			"vec3 textureColor = colorr.xyz;"

			"vec3 ambient = light.ambient[i]*textureColor;"

			"vec3 lightDir = normalize(light.position[i]-fragPos);"
			"float diff = max(dot(normalFrag,lightDir),0.0);"
			"vec3 diffuse = light.diffuse[i] * (diff * textureColor);"

			"vec3 viewDir = normalize(viewPos - fragPos);"
			"vec3 reflectDir = reflect(-lightDir,normalFrag);"
			"float spec = pow(max(dot(viewDir,reflectDir),0.0),64.0);"
			"vec3 specular = light.specular[i] * (spec*vec3(0.1,0.1,0.1));"
			"res += attenuation*ambient + attenuation*diffuse+ attenuation*specular;"
			"}"
			"color = vec4(res,1);"
			"}";

			const auto vertex = compileShader(vertexCode, GL_VERTEX_SHADER);
			const auto frag = compileShader(fragCode, GL_FRAGMENT_SHADER);
			auto program = compileProgram(vertex,frag);
			glUseProgram(program);

			std::unordered_map<std::string, GLuint> uniformLocations;
			uniformLocations["textureSampler"] = glGetUniformLocation(program, "textureSampler");

			uniformLocations["light.position"] = glGetUniformLocation(program, "light.position");
			uniformLocations["light.ambient"] = glGetUniformLocation(program, "light.ambient");
			uniformLocations["light.diffuse"] = glGetUniformLocation(program, "light.diffuse");
			uniformLocations["light.specular"] = glGetUniformLocation(program, "light.specular");
			uniformLocations["light.constant"] = glGetUniformLocation(program, "light.constant");
			uniformLocations["light.linear"] = glGetUniformLocation(program, "light.linear");
			uniformLocations["light.quadratic"] = glGetUniformLocation(program, "light.quadratic");
			uniformLocations["lightCount"] = glGetUniformLocation(program, "lightCount");


			uniformLocations["viewPos"] = glGetUniformLocation(program, "viewPos");
			uniformLocations["model"] = glGetUniformLocation(program, "model");
			uniformLocations["view"] = glGetUniformLocation(program, "view");
			uniformLocations["projection"] = glGetUniformLocation(program, "projection");

			programs[LIGTH_PROGRAM] = std::make_unique<ProgramDescription>(ProgramDescription(program, uniformLocations));
	}

	//MATERIAL_PROGRAM
	{
	
		const  char * vertexCode =
			"#version 330 core\n"
			"layout(location = 0) in vec3 pointPosition;"
			"layout(location = 1) in vec3 normal;"
			"out vec3 normalFrag;"
			"out vec3 fragPos;"


			"uniform mat4 model;"
			"uniform mat4 view;"
			"uniform mat4 projection;"
			"void main(){"
				"normalFrag = (model*vec4(normal,0.0)).xyz;;"
				"fragPos = vec3(model*vec4(pointPosition,1.0));"
				"gl_Position = projection*view*model*vec4(pointPosition,1.0);"
			"}";

		const char* fragCode =
			"#version 330 core\n"
			"struct Material {"
			"vec3 ambient;"
			"vec3 diffuse;"
			"vec3 specular;"
			"float shininess;"
			"};\n"
			"#define MAX_LIGHT 5\n"
			"struct Light{"
			"vec3 position[MAX_LIGHT];"
			"vec3 ambient[MAX_LIGHT];"
			"vec3 diffuse[MAX_LIGHT];"
			"vec3 specular[MAX_LIGHT];"
			"float constant[MAX_LIGHT];"
			"float linear[MAX_LIGHT];"
			"float quadratic[MAX_LIGHT];"
			"};"

			"out vec4 color;"
			"in vec3 normalFrag;"
			"in vec3 fragPos;"

			"uniform vec3 viewPos;"
			"uniform Light light;"
			"uniform int lightCount;"
			"uniform Material material;"
			"void main(){"

			"vec3 result = vec3(0,0,0);"

			"for(int i = 0; i < lightCount;i++){"

			"float distance = length(light.position[i] - fragPos);"
			"float attenuation = 1.0/(light.constant[i]+light.linear[i]*distance"
			"+ light.quadratic[i]*(distance*distance));"

			"vec3 ambient = light.ambient[i]*material.ambient;"

			"vec3 lightDir = normalize(light.position[i]-fragPos);"
			"float diff = max(dot(normalFrag,lightDir),0.0);"
			"vec3 diffuse = light.diffuse[i] * (diff * material.diffuse);"

			"vec3 viewDir = normalize(viewPos - fragPos);"
			"vec3 reflectDir = reflect(-lightDir,normalFrag);"
			"float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);"
			"vec3 specular = light.specular[i] * (spec*material.specular);"
			"result += attenuation*ambient + attenuation*diffuse+ attenuation*specular;"

			"}"
			"color = vec4(result,1.0);"

			"}";
		const auto vertex = compileShader(vertexCode, GL_VERTEX_SHADER);
		const auto frag = compileShader(fragCode, GL_FRAGMENT_SHADER);
		auto program = compileProgram(vertex, frag);
	
	
		std::unordered_map<std::string, GLuint> uniformLocations;

		uniformLocations["light.position"] = glGetUniformLocation(program, "light.position");
		uniformLocations["light.ambient"] = glGetUniformLocation(program, "light.ambient");
		uniformLocations["light.diffuse"] = glGetUniformLocation(program, "light.diffuse");
		uniformLocations["light.specular"] = glGetUniformLocation(program, "light.specular");
		uniformLocations["light.constant"] = glGetUniformLocation(program, "light.constant");
		uniformLocations["light.linear"] = glGetUniformLocation(program, "light.linear");
		uniformLocations["light.quadratic"] = glGetUniformLocation(program, "light.quadratic");
		uniformLocations["lightCount"] = glGetUniformLocation(program, "lightCount");


		uniformLocations["material.ambient"] = glGetUniformLocation(program, "material.ambient");
		uniformLocations["material.diffuse"] = glGetUniformLocation(program, "material.diffuse");
		uniformLocations["material.specular"] = glGetUniformLocation(program, "material.specular");
		uniformLocations["material.shininess"] = glGetUniformLocation(program, "material.shininess");




		uniformLocations["viewPos"] = glGetUniformLocation(program, "viewPos");

		uniformLocations["model"] = glGetUniformLocation(program, "model");
		uniformLocations["view"] = glGetUniformLocation(program, "view");
		uniformLocations["projection"] = glGetUniformLocation(program, "projection");

		programs[MATERIAL_PROGRAM] = std::make_unique<ProgramDescription>(ProgramDescription(program, uniformLocations));

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
