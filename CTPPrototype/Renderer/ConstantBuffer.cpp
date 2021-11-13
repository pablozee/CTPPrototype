#include "ConstantBuffer.h"
#include "Buffer.h"

namespace ConstantBuffer
{
	void CreateConstantBuffer(D3D12Global& d3d, ID3D12Resource** buffer, UINT64 size)
	{
		D3D12BufferCreateInfo bufferInfo((size + 255) & ~255, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);
		Buffer::CreateBuffer(d3d, bufferInfo, buffer);
	}
}