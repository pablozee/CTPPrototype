#include "PSO.h"

using namespace std;

namespace PSO
{
	void CreatePipelineStateObject(D3D12Global& d3d, DXRGlobal& dxr)
	{
		// Need 10 subobjects:
		// 1 for RGS program
		// 1 for Miss program
		// 1 for CHS program
		// 1 for Hit Group
		// 2 for RayGen Root Signature (root-signature and association)
		// 2 for Shader Config (config and association)
		// 1 for Global Root Signature
		// 1 for Pipeline Config

		UINT index = 0;
		vector<D3D12_STATE_SUBOBJECT> subobjects;
		subobjects.resize(10);

		// Add state subobject for the RGS
		D3D12_EXPORT_DESC rgsExportDesc = {};
		rgsExportDesc.Name = L"RayGen_12";
		rgsExportDesc.ExportToRename = L"RayGen";
		rgsExportDesc.Flags = D3D12_EXPORT_FLAG_NONE;

		D3D12_DXIL_LIBRARY_DESC	rgsLibDesc = {};
		rgsLibDesc.DXILLibrary.BytecodeLength = dxr.rgs.blob->GetBufferSize();
		rgsLibDesc.DXILLibrary.pShaderBytecode = dxr.rgs.blob->GetBufferPointer();
		rgsLibDesc.NumExports = 1;
		rgsLibDesc.pExports = &rgsExportDesc;

		D3D12_STATE_SUBOBJECT rgs = {};
		rgs.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
		rgs.pDesc = &rgsLibDesc;

		subobjects[index++] = rgs;

		// Add state subobject for the Miss shader
		D3D12_EXPORT_DESC msExportDesc = {};
		msExportDesc.Name = L"Miss_5";
		msExportDesc.ExportToRename = L"Miss";
		msExportDesc.Flags = D3D12_EXPORT_FLAG_NONE;

		D3D12_DXIL_LIBRARY_DESC	msLibDesc = {};
		msLibDesc.DXILLibrary.BytecodeLength = dxr.miss.blob->GetBufferSize();
		msLibDesc.DXILLibrary.pShaderBytecode = dxr.miss.blob->GetBufferPointer();
		msLibDesc.NumExports = 1;
		msLibDesc.pExports = &msExportDesc;

		D3D12_STATE_SUBOBJECT ms = {};
		ms.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
		ms.pDesc = &msLibDesc;

		subobjects[index++] = ms;

		// Add state subobject for the Closest Hit shader
		D3D12_EXPORT_DESC chsExportDesc = {};
		chsExportDesc.Name = L"ClosestHit_76";
		chsExportDesc.ExportToRename = L"ClosestHit";
		chsExportDesc.Flags = D3D12_EXPORT_FLAG_NONE;

		D3D12_DXIL_LIBRARY_DESC	chsLibDesc = {};
		chsLibDesc.DXILLibrary.BytecodeLength = dxr.hit.chs.blob->GetBufferSize();
		chsLibDesc.DXILLibrary.pShaderBytecode = dxr.hit.chs.blob->GetBufferPointer();
		chsLibDesc.NumExports = 1;
		chsLibDesc.pExports = &chsExportDesc;

		D3D12_STATE_SUBOBJECT chs = {};
		chs.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
		chs.pDesc = &chsLibDesc;

		subobjects[index++] = chs;

		// Add a state subobject for the hit group
		D3D12_HIT_GROUP_DESC hitGroupDesc = {};
		hitGroupDesc.ClosestHitShaderImport = L"ClosestHit_76";
		hitGroupDesc.HitGroupExport = L"HitGroup";

		D3D12_STATE_SUBOBJECT hitGroup = {};
		hitGroup.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
		hitGroup.pDesc = &hitGroupDesc;

		subobjects[index++] = hitGroup;

		// Add a state subobject for the shader payload configuration
		D3D12_RAYTRACING_SHADER_CONFIG shaderDesc = {};
		shaderDesc.MaxPayloadSizeInBytes = sizeof(XMFLOAT4);	// RGB and HitT
		shaderDesc.MaxAttributeSizeInBytes = D3D12_RAYTRACING_MAX_ATTRIBUTE_SIZE_IN_BYTES;

		D3D12_STATE_SUBOBJECT shaderConfigObject = {};
		shaderConfigObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
		shaderConfigObject.pDesc = &shaderDesc;

		subobjects[index++] = shaderConfigObject;

		// Create a list of the shader export names that use the payload
		const WCHAR* shaderExports[] = { L"RayGen_12", L"Miss_5", L"HitGroup" };

		// Add a state subobject for the association between shaders and the payload
		D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION shaderPayloadAssociation = {};
		shaderPayloadAssociation.NumExports = _countof(shaderExports);
		shaderPayloadAssociation.pExports = shaderExports;
		shaderPayloadAssociation.pSubobjectToAssociate = &subobjects[(index - 1)];

		D3D12_STATE_SUBOBJECT shaderPayloadAssociationObject = {};
		shaderPayloadAssociationObject.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
		shaderPayloadAssociationObject.pDesc = &shaderPayloadAssociation;

		subobjects[index++] = shaderPayloadAssociationObject;

		// Add a state subobject for the shared root signature 
		D3D12_STATE_SUBOBJECT rayGenRootSigObject = {};
		rayGenRootSigObject.Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
		rayGenRootSigObject.pDesc = &dxr.rgs.pRootSignature;

		subobjects[index++] = rayGenRootSigObject;

		// Create a list of the shader export names that use the root signature
		const WCHAR* rootSigExports[] = { L"RayGen_12", L"HitGroup", L"Miss_5" };

		// Add a state subobject for the association between the RayGen shader and the local root signature
		D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION rayGenShaderRootSigAssociation = {};
		rayGenShaderRootSigAssociation.NumExports = _countof(rootSigExports);
		rayGenShaderRootSigAssociation.pExports = rootSigExports;
		rayGenShaderRootSigAssociation.pSubobjectToAssociate = &subobjects[(index - 1)];

		D3D12_STATE_SUBOBJECT rayGenShaderRootSigAssociationObject = {};
		rayGenShaderRootSigAssociationObject.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
		rayGenShaderRootSigAssociationObject.pDesc = &rayGenShaderRootSigAssociation;

		subobjects[index++] = rayGenShaderRootSigAssociationObject;

		D3D12_STATE_SUBOBJECT globalRootSig;
		globalRootSig.Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
		globalRootSig.pDesc = &dxr.miss.pRootSignature;

		subobjects[index++] = globalRootSig;

		// Add a state subobject for the ray tracing pipeline config
		D3D12_RAYTRACING_PIPELINE_CONFIG pipelineConfig = {};
		pipelineConfig.MaxTraceRecursionDepth = 1;

		D3D12_STATE_SUBOBJECT pipelineConfigObject = {};
		pipelineConfigObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
		pipelineConfigObject.pDesc = &pipelineConfig;

		subobjects[index++] = pipelineConfigObject;

		// Describe the Ray Tracing Pipeline State Object
		D3D12_STATE_OBJECT_DESC pipelineDesc = {};
		pipelineDesc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
		pipelineDesc.NumSubobjects = static_cast<UINT>(subobjects.size());
		pipelineDesc.pSubobjects = subobjects.data();

		// Create the RT Pipeline State Object (RTPSO)
		HRESULT hr = d3d.device->CreateStateObject(&pipelineDesc, IID_PPV_ARGS(&dxr.rtpso));
		Utils::Validate(hr, L"Error: failed to create state object!");

#if NAME_D3D_RESOURCES
		dxr.rtpso->SetName(L"DXR Pipeline State Object");
#endif

		// Get the RTPSO properties
		hr = dxr.rtpso->QueryInterface(IID_PPV_ARGS(&dxr.rtpsoInfo));
		Utils::Validate(hr, L"Error: failed to get RTPSO info object!");
	}
}