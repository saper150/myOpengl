#pragma once
#include "stdafx.h"
#include <fstream>
#include <streambuf>
#include <rapidxml\rapidxml_print.hpp>
#include <sstream>
#include "TexturedModel.h"

using namespace rapidxml;

template<typename T,int size>
std::vector<T> parseArray(const char* toParse) {

	std::vector<T> v;

	char buffer[50];
	T vec;

	int bufferIndex = 0;
	int vertexIndex = 0;


	while (*toParse !='\0')
	{
		char a = *toParse;
		if (*toParse != ' ') {
			buffer[bufferIndex] = (*toParse);
			bufferIndex++;
		}
		else {
			 
			buffer[bufferIndex] = 0;
			vec[vertexIndex] = std::atof(buffer);
			vertexIndex++;

			if (vertexIndex == size) {
				vertexIndex = 0;
				v.emplace_back(vec);
			}

			
			bufferIndex = 0;
		}

		toParse++;
	}
	buffer[bufferIndex] = '\0';
	vec[size - 1] = std::atof(buffer);
	v.emplace_back(vec);
	return v;
}



template<typename T>
std::vector<T> unpack(std::vector<T> in,std::vector<int> indexes) {
	std::vector<T> unpacked;
	unpacked.resize(indexes.size());
	for (auto index : indexes) {
		unpacked.emplace_back(in[index]);
	}
	return unpacked;

}


std::vector<std::vector<int>> parseIndeces(const char * toParse, int count);
std::vector<TextureVertex> pack(const std::vector<glm::vec3>& pos, const std::vector<glm::vec2>& uv);

TextureModelPrototype loadModel(const std::string& modelPath, const std::string& texturePath);