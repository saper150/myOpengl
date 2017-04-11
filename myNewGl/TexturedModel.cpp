#include "stdafx.h"
#include "TexturedModel.h"


TextureModelPrototype createSpritePrototype(const std::string texturePath) {
	std::vector<TextureVertex> v = {
		{ { -1,1,0 },{ 0,1 } },
		{ { 1,-1,0 },{ 1,0 } },
		{ { -1,-1,0 },{ 0,0 } },
		{ { -1,1,0 },{ 0,1 } },
		{ { 1,1,0 },{ 1,1 } },
		{ { 1,-1,0 },{ 1,0 } }
	};
	return TextureModelPrototype(Shaders::getInstance().getProgram(Shaders::vertex::UV, Shaders::fragment::TEXTURE), v, texturePath);
}