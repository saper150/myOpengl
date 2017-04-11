#include "stdafx.h"
#include "utils.h"
#include <soil\SOIL.h>
#include <fstream>

bool fileExists(const std::string& name) {
	std::ifstream f(name.c_str());
	return f.good();
}

GLuint loadTexture(const std::string& path, int magFilter, int minFilter) {
	if (!fileExists(path)) {
		throw std::runtime_error("cant find texture: "+path);
	}


	int width, height;
	auto deleater = [](uint8_t* image) { SOIL_free_image_data(image); };
	std::unique_ptr<uint8_t, decltype(deleater)> image(SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_RGBA), deleater);

	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.get());
	glGenerateMipmap(GL_TEXTURE_2D);

	return textureId;

}

GLuint createVertexArray()
{
	GLuint vertexArray;
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);
	return vertexArray;
}

GLuint loadTexture(const std::string& path) {
	return loadTexture(path,GL_LINEAR,GL_LINEAR_MIPMAP_LINEAR);
}

glm::vec3 extractTranslation(const glm::mat4& model) {
	glm::vec3 translation;
	translation.x = model[0][3];
	translation.y = model[1][3];
	translation.x = model[2][3];
	return translation;

}