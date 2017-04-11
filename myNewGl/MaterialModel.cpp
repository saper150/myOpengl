#include "stdafx.h"
#include "MaterialModel.h"

Material createMaterial(Materials material)
{


	static std::array<Material, 1> materialDefinitions;

	materialDefinitions[PEARL] = {
		{ 0.25f ,0.20725f,0.20725f },
		{ 1.f ,0.829f,0.829f },
		{ 0.296648f ,0.296648f,0.296648f },
		128*0.088f
	};
	return materialDefinitions[material];
}
