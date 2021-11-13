#pragma once

#include "DirectXStructures.h"

namespace Texture
{
	void CreateTexture(D3D12Global& d3d, D3D12Resources& resources, Material& material);

	void UploadTexture(D3D12Global& d3d, ID3D12Resource* destResource, ID3D12Resource* srcResource, const TextureInfo& texture);
}