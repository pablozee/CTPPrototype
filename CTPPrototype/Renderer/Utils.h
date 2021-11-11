#pragma once

#include "Structures.h"

namespace Utils
{
	void LoadModel(std::string filepath, Model& model, Material& material);

	void Validate(HRESULT hr, LPWSTR message);
}