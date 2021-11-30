#include "Common.hlsl"

[shader("closesthit")]
void ClosestHit(inout HitInfo payload, Attributes attrib)
{
	uint triangleIndex = PrimitiveIndex();
	float3 barycentrics = float3((1.0f - attrib.uv.x - attrib.uv.y), attrib.uv.x, attrib.uv.y);
	VertexAttributes vertex = GetVertexAttributes(triangleIndex, barycentrics);
	
	float3 color = Materials[int(vertex.materialIndex)].diffuse;
//	float3 color = vertex.materialIndex;

	/*
	if (MaterialCB[0].useTex == 1)
	{
		int2 coord = floor(vertex.uv * MaterialCB[0].textureResolution.x);
		float3 color = albedo.Load(int3(coord, 0)).rgb;
	}
	*/

//	color = float3(vertex.materialIndex, vertex.materialIndex, vertex.materialIndex);

	payload.ShadedColorAndHitT = float4(color, RayTCurrent());
}