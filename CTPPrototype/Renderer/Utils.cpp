#include "Utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <fstream>
#include <shellapi.h>
#include <unordered_map>

namespace std
{
	void hash_combine(size_t& seed, size_t hash)
	{
		hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= hash;
	}

	template<> struct hash<Vertex>
	{
		size_t operator()(Vertex const& vertex) const
		{
			size_t seed = 0;
			hash<float> hasher;
			hash_combine(seed, hasher(vertex.position.x));
			hash_combine(seed, hasher(vertex.position.y));
			hash_combine(seed, hasher(vertex.position.z));

			hash_combine(seed, hasher(vertex.uv.x));
			hash_combine(seed, hasher(vertex.uv.y));

			return seed;
		}
	};
}

using namespace std;

namespace Utils
{
	void Validate(HRESULT hr, LPWSTR msg)
	{
		if (FAILED(hr))
		{
			MessageBox(NULL, msg, L"Error", MB_OK);
			PostQuitMessage(EXIT_FAILURE);
		}
	}

	void LoadModel(string filepath, std::vector<Model>& modelsVec, std::vector<Material>& mats)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;

		Model model;
		modelsVec.reserve(modelsVec.size() + 1);


		// Load the OBJ and MTL files
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filepath.c_str(), "materials\\"))
		{
			throw std::runtime_error(err);
		}
		// Get the first material
		// Only support a single material right now

		mats.reserve(materials.size());

		for (int i = 0; i < materials.size(); i++)
		{
			Material material;
			material.name = materials[i].name;
			material.texturePath = materials[i].diffuse_texname;
			material.diffuse = XMFLOAT3(materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);
			material.useTex = 0;
			mats.push_back(material);
		}

		unordered_map<Vertex, uint32_t> uniqueVertices = {};

		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces(polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					Vertex vertex = {};
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

					vertex.position =
					{
					 attrib.vertices[3 * size_t(idx.vertex_index) + 0],
					 attrib.vertices[3 * size_t(idx.vertex_index) + 1],
					 attrib.vertices[3 * size_t(idx.vertex_index) + 2]
					};

					vertex.uv =
					{
						attrib.texcoords[2 * size_t(idx.texcoord_index) + 0],
						attrib.texcoords[2 * size_t(idx.texcoord_index) + 1]
					};

					vertex.normal =
					{
						attrib.normals[3 * size_t(idx.normal_index) + 0],
						attrib.normals[3 * size_t(idx.normal_index) + 1],
						attrib.normals[3 * size_t(idx.normal_index) + 2]
					};


					vertex.materialIndex = float(shapes[s].mesh.material_ids[f]);

					if (uniqueVertices.count(vertex) == 0)
					{
						uniqueVertices[vertex] = static_cast<uint32_t>(model.vertices.size());
						model.vertices.push_back(vertex);
					}

					model.indices.push_back(uniqueVertices[vertex]);
				}
				index_offset += fv;
			}
		}

		modelsVec.push_back(model);
	}

	void FormatTexture(TextureInfo& info, UINT8* pixels)
	{
		const UINT numPixels = (info.width * info.height);
		const UINT oldStride = info.stride;
		const UINT oldSize = (numPixels * info.stride);

		const UINT newStride = 4;				// uploading textures to GPU as DXGI_FORMAT_R8G8B8A8_UNORM
		const UINT newSize = (numPixels * newStride);
		info.pixels.resize(newSize);

		for (UINT i = 0; i < numPixels; i++)
		{
			info.pixels[i * newStride] = pixels[i * oldStride];		// R
			info.pixels[i * newStride + 1] = pixels[i * oldStride + 1];	// G
			info.pixels[i * newStride + 2] = pixels[i * oldStride + 2];	// B
			info.pixels[i * newStride + 3] = 0xFF;							// A (always 1)
		}

		info.stride = newStride;
	}

	TextureInfo LoadTexture(string filepath)
	{
		TextureInfo result = {};

		// Load image pixels with stb_image
		UINT8* pixels = stbi_load(filepath.c_str(), &result.width, &result.height, &result.stride, STBI_default);
		if (!pixels)
		{
			throw runtime_error("Error: failed to load image!");
		}

		FormatTexture(result, pixels);
		stbi_image_free(pixels);
		return result;
	}
}