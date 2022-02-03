#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include <memory>
#include "Device.h"
#include "Mesh.h"
class Assest
{
public:
	Assest() = default;
	~Assest();
	void ReleaseBuffer();
	void prepare(Device* device, VkCommandPool cmdpool);
	//std::shared_ptr<VertexBuffer> vbuffer;
	std::vector<Mesh> meshes;
};
