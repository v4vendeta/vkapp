#include "Descriptors.h"

#include <unordered_map>

#include "UniformBuffer.h"

namespace Horizon {

	DescriptorSet::DescriptorSet(std::shared_ptr<Device> device, std::shared_ptr<DescriptorSetInfo> setInfo) :mDevice(device), mDescriptorSetInfo(setInfo)
	{
		createDescriptorSetLayout();
		createDescriptorPool();
	}

	DescriptorSet::~DescriptorSet()
	{
		vkDestroyDescriptorPool(mDevice->get(), mDescriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(mDevice->get(), mSetLayout, nullptr);
	}

	void DescriptorSet::createDescriptorSetLayout()
	{

		std::vector<VkDescriptorSetLayoutBinding> bindings(mDescriptorSetInfo->bindingCount);
		for (u32 binding = 0; binding < mDescriptorSetInfo->bindingCount; binding++) {
			bindings[binding].binding = binding;
			bindings[binding].descriptorType = mDescriptorSetInfo->types[binding];
			bindings[binding].descriptorCount = 1;
			bindings[binding].stageFlags = mDescriptorSetInfo->stageFlags[binding];
		}
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = bindings.size();
		layoutInfo.pBindings = bindings.data();

		printVkError(vkCreateDescriptorSetLayout(mDevice->get(), &layoutInfo, nullptr, &mSetLayout), "create descriptor set layout!");

	}

	void DescriptorSet::updateDescriptorSet(const DescriptorSetUpdateDesc& desc)
	{

		// update descriptor set
		std::vector<VkWriteDescriptorSet> descriptorWrites(mDescriptorSetInfo->bindingCount);
		for (u32 binding = 0; binding < mDescriptorSetInfo->bindingCount; binding++) {
			descriptorWrites[binding].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[binding].pNext = nullptr;
			descriptorWrites[binding].dstSet = mSet;
			descriptorWrites[binding].dstBinding = binding;
			descriptorWrites[binding].dstArrayElement = 0;
			descriptorWrites[binding].descriptorCount = 1;
			descriptorWrites[binding].descriptorType = mDescriptorSetInfo->types[binding];
			switch (descriptorWrites[binding].descriptorType)
			{
			case VK_DESCRIPTOR_TYPE_SAMPLER:
			case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
			case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
			case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
				descriptorWrites[binding].pImageInfo = &desc.descriptorMap.at(binding).get()->imageDescriptorInfo;
				break;
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
				descriptorWrites[binding].pBufferInfo = &desc.descriptorMap.at(binding).get()->bufferDescriptrInfo;
				break;
			default:
				break;
			}
		}
		vkUpdateDescriptorSets(mDevice->get(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
	}

	VkDescriptorSetLayout DescriptorSet::getLayout() {
		return mSetLayout;
	}

	VkDescriptorSet DescriptorSet::get()
	{
		return mSet;
	}

	void DescriptorSet::allocateDescriptorSet() {
		if (mSet != VK_NULL_HANDLE) {
			printVkError(vkFreeDescriptorSets(mDevice->get(), mDescriptorPool, 1, &mSet));
		}
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = mDescriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &mSetLayout;
		printVkError(vkAllocateDescriptorSets(mDevice->get(), &allocInfo, &mSet));
		if (!mSet) {
			spdlog::error("failed to allocate descriptorset");
		}
	}

	void DescriptorSet::createDescriptorPool() {

		std::unordered_map<VkDescriptorType, u32> descriptorTypeMap;

		for (u32 binding = 0; binding < mDescriptorSetInfo->bindingCount; binding++) {
			descriptorTypeMap[mDescriptorSetInfo->types[binding]]++;
		}

		std::vector<VkDescriptorPoolSize> poolSizes(descriptorTypeMap.size());

		u32 i = 0;
		for (auto& type : descriptorTypeMap) {
			poolSizes[i++] = VkDescriptorPoolSize{type.first, type.second};
		}

		VkDescriptorPoolCreateInfo poolInfo{};

		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.pNext = nullptr;
		poolInfo.flags = 0;
		poolInfo.maxSets = 1;
		poolInfo.poolSizeCount = static_cast<u32>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		printVkError(vkCreateDescriptorPool(mDevice->get(), &poolInfo, nullptr, &mDescriptorPool), "create descriptor pool", logLevel::debug);

	}

	void DescriptorSetInfo::addBinding(VkDescriptorType type, VkShaderStageFlags stage)
	{
		bindingCount++;
		types.push_back(type);
		stageFlags.push_back(stage);
	}


	void DescriptorSetUpdateDesc::bindResource(u32 binding, std::shared_ptr<DescriptorBase> buffer)
	{
		descriptorMap[binding] = buffer;
	}

}