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


std::vector<std::vector<int>> parseIndeces(const char * toParse,int count) {
	std::vector<std::vector<int>> result(count);
	result.resize(count);
	int offsetIndex = 0;
	int bufferIndex = 0;
	char buffer[5];

	while (*toParse != '\0') {
		if (*toParse == ' ') {
			buffer[bufferIndex] = '\0';
			bufferIndex = 0;
			result[offsetIndex].emplace_back(std::stoi(buffer));
			offsetIndex++;
			if (offsetIndex == count)
				offsetIndex = 0;
		}
		else {
			buffer[bufferIndex] = *toParse;
			bufferIndex++;
		}
		toParse++;
	}
	buffer[bufferIndex] = '\0';
	result[count-1].emplace_back(std::stoi(buffer));
	return result;

}

std::vector<TextureVertex> pack(const std::vector<glm::vec3>& pos, const std::vector<glm::vec2>& uv) {
	std::vector<TextureVertex> res(pos.size());
	for (size_t i = 0; i < pos.size(); i++)
	{
		res.push_back({pos[i],uv[i]});
	}
	return res;

}

typedef SingleTransformationWraper<ProgramTexture> Model;
Model loadModel(const std::string& modelPath, const std::string& texturePath) {
	std::ifstream file(modelPath,std::ifstream::in);
	std::string str;

	auto ss = std::ostringstream{};

	ss << file.rdbuf();
	auto s = ss.str();

	xml_document<> doc;
	
	doc.parse<0>(&s[0]);

	xml_node<>* mesh = doc.first_node("COLLADA")
		->first_node("library_geometries")
		->first_node("geometry")
		->first_node("mesh");

	std::vector<glm::vec3> position;
	std::vector<glm::vec2> uv;

	std::vector<int> positionIndexes;

	for (xml_node<>* node = mesh->first_node(); node; node = node->next_sibling()) {
		if (std::string(node->name()) == "source") {

			if (std::string(node->first_attribute("id")->value()).find("position") != std::string::npos) {
				xml_node<>* arr = node->first_node("float_array");
				position = parseArray<glm::vec3,3>(arr->value());
			}
			if (std::string(node->first_attribute("id")->value()).find("mesh-map") != std::string::npos) {
				xml_node<>* arr = node->first_node("float_array");
				uv = parseArray<glm::vec2, 2>(arr->value());
			}

		}
	}

	const auto indecesRAW = mesh
		->first_node("polylist")
		->first_node("p")
		->value();
	const auto indeces = parseIndeces(indecesRAW, 3);

	const auto positions = unpack<glm::vec3>(position,indeces[0]);
	const auto uvs = unpack<glm::vec2>(uv,indeces[2]);

	const ProgramTexture program(Shaders::getInstance().getProgram(Shaders::vertex::UV, Shaders::fragment::TEXTURE),pack(positions,uvs),texturePath);

	return Model(program);
}