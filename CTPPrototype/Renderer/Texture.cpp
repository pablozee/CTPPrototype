#include "Texture.h"

namespace Texture
{
	void CreateTexture(D3D12Global& d3d, D3D12Resources& resources, Material& material)
	{
		// Load the texture
		TextureInfo texture = Utils::LoadTexture(material.texturePath);
		material.textureResolution = static_cast<float>(texture.width);

		// Describe the texture
		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.Width = texture.width;
		textureDesc.Height = texture.height;
		textureDesc.MipLevels = 1;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		// Create the texture resource
		HRESULT hr = d3d.device->CreateCommittedResource(&DefaultHeapProperties, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&resources.texture));
		Utils::Validate(hr, L"Error: failed to create texture!");

#if NAME_D3D_RESOURCES
		resources.texture->SetName(L"Texture");
#endif

		// Describe the resource
		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Width = (texture.width * texture.height * texture.stride);
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

		// Create the upload heap
		hr = d3d.device->CreateCommittedResource(&UploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resources.textureUploadResource));
		Utils::Validate(hr, L"Error: failed to create texture upload heap!");

#if NAME_D3D_RESOURCES
		resources.textureUploadResource->SetName(L"Texture Upload Buffer");
#endif

		// Upload the texture to the GPU
		UploadTexture(d3d, resources.texture, resources.textureUploadResource, texture);
	}

	/**
 * Copy a texture from the CPU to the GPU upload heap, then schedule a copy to the default heap.
 */
	void UploadTexture(D3D12Global& d3d, ID3D12Resource* destResource, ID3D12Resource* srcResource, const TextureInfo& texture)
	{
		// Copy the pixel data to the upload heap resource
		UINT8* pData;
		HRESULT hr = srcResource->Map(0, nullptr, reinterpret_cast<void**>(&pData));
		memcpy(pData, texture.pixels.data(), texture.width * texture.height * texture.stride);
		srcResource->Unmap(0, nullptr);

		// Describe the upload heap resource location for the copy
		D3D12_SUBRESOURCE_FOOTPRINT subresource = {};
		subresource.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		subresource.Width = texture.width;
		subresource.Height = texture.height;
		subresource.RowPitch = (texture.width * texture.stride);
		subresource.Depth = 1;

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
		footprint.Offset = texture.offset;
		footprint.Footprint = subresource;

		D3D12_TEXTURE_COPY_LOCATION source = {};
		source.pResource = srcResource;
		source.PlacedFootprint = footprint;
		source.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

		// Describe the default heap resource location for the copy
		D3D12_TEXTURE_COPY_LOCATION destination = {};
		destination.pResource = destResource;
		destination.SubresourceIndex = 0;
		destination.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

		// Copy the buffer resource from the upload heap to the texture resource on the default heap
		d3d.cmdList->CopyTextureRegion(&destination, 0, 0, 0, &source, nullptr);

		// Transition the texture to a shader resource
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = destResource;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		d3d.cmdList->ResourceBarrier(1, &barrier);
	}
}