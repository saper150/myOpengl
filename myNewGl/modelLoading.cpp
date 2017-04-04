#include "stdafx.h"
#include "modelLoading.h"

struct ModelData {
	const std::vector<glm::vec3> pos;
	const std::vector<glm::vec3> normals;
	const std::vector<glm::vec2> uvs;
};



ModelData getModelData(std::string modelPath) {


	std::ifstream file(modelPath, std::ifstream::in);
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
	std::vector<glm::vec3> normal;


	std::vector<int> positionIndexes;

	for (xml_node<>* node = mesh->first_node(); node; node = node->next_sibling()) {
		if (std::string(node->name()) == "source") {

			if (std::string(node->first_attribute("id")->value()).find("position") != std::string::npos) {
				xml_node<>* arr = node->first_node("float_array");
				position = parseArray<glm::vec3, 3>(arr->value());
			}
			if (std::string(node->first_attribute("id")->value()).find("mesh-map") != std::string::npos) {
				xml_node<>* arr = node->first_node("float_array");
				uv = parseArray<glm::vec2, 2>(arr->value());
			}
			if (std::string(node->first_attribute("id")->value()).find("mesh-normals") != std::string::npos) {
				xml_node<>* arr = node->first_node("float_array");
				normal = parseArray<glm::vec3, 3>(arr->value());
			}

		}
	}
	const auto indecesRAW = mesh
		->first_node("polylist")
		->first_node("p")
		->value();
	const auto indeces = parseIndeces(indecesRAW, 3);

	const auto positions = unpack<glm::vec3>(position, indeces[0]);
	const auto normals = unpack<glm::vec3>(normal, indeces[1]);
	const auto uvs = unpack<glm::vec2>(uv, indeces[2]);


	return{ positions,normals,uvs };

}

std::vector<LigthVertex> pack(const std::vector<glm::vec3>& pos, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& uv) {
	std::vector<LigthVertex> res(pos.size());
	for (size_t i = 0; i < pos.size(); i++)
	{
		res.push_back({ pos[i],normals[i],uv[i] });
	}
	return res;
}

std::vector<TextureVertex> pack(const std::vector<glm::vec3>& pos, const std::vector<glm::vec2>& uv) {
	std::vector<TextureVertex> res(pos.size());
	for (size_t i = 0; i < pos.size(); i++)
	{
		res.push_back({ pos[i],uv[i] });
	}
	return res;
}

TextureModelPrototype loadModel(const std::string& modelPath, const std::string& texturePath) {

	const auto modelData = getModelData(modelPath);
	return TextureModelPrototype(Shaders::getInstance().getProgram(Shaders::vertex::UV, Shaders::fragment::TEXTURE), pack(modelData.pos, modelData.uvs), texturePath);
}

LigthModelPrototype loadModelLigth(const std::string& modelPath, const std::string& texturePath) {
	const auto modelData = getModelData(modelPath);
	return LigthModelPrototype(pack(modelData.pos, modelData.normals, modelData.uvs),texturePath);

}








std::vector<std::vector<int>> parseIndeces(const char * toParse, int count) {
	std::vector<std::vector<int>> result(count);
	result.resize(count);
	int offsetIndex = 0;
	int bufferIndex = 0;
	char buffer[20];

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
	result[count - 1].emplace_back(std::stoi(buffer));
	return result;

}