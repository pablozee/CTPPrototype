struct HitInfo
{
	float4 ShadedColorAndHitT;
};

struct Attributes
{
	float2 uv;
};

struct Material 
{
	float4 textureResolution;
	float3 diffuse;
	int useTex;
};
// Constant Buffers

cbuffer ViewCB : register(b0)
{
	matrix view;
	float4 viewOriginAndTanHalfFovY;
	float2 resolution;
};



// Resources

RWTexture2D<float4> RTOutput				: register(u0);
RaytracingAccelerationStructure SceneBVH : register(t0);

ByteAddressBuffer indices					: register(t1);
ByteAddressBuffer vertices					: register(t2);
StructuredBuffer<Material> Materials		: register(t3);
Texture2D<float4> albedo					: register(t4);

// Helper Functions

struct VertexAttributes
{
	float3 position;
	float2 uv;
	float3 normal;
	float materialIndex;
};

uint3 GetIndices(uint triangleIndex)
{
	uint baseIndex = (triangleIndex * 3);
	int address = (baseIndex * 4);
	return indices.Load3(address);
}

VertexAttributes GetVertexAttributes(uint triangleIndex, float3 barycentrics)
{
	uint3 indices = GetIndices(triangleIndex);
	VertexAttributes v;
	v.position = float3(0, 0, 0);
	v.uv = float2(0, 0);
	v.normal = float3(0, 0, 0);
	v.materialIndex = 0.0f;

	for (uint i = 0; i < 3; i++)
	{
		int address = (indices[i] * 9) * 4;
		v.position += asfloat(vertices.Load3(address)) * barycentrics[i];
		address += (3 * 4);
		v.uv += asfloat(vertices.Load2(address)) * barycentrics[i];
		address += (2 * 4);
		v.normal += asfloat(vertices.Load3(address)) * barycentrics[i];
		address += (3 * 4);
		v.materialIndex = asfloat(vertices.Load(address));
	}

	return v;
}