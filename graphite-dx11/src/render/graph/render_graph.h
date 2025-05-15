#pragma once

#include <vector>
#include <functional>
#include <string>
#include "render/context/frame_render_context.h"

enum class AccessType
{
	Read,
	Write,
	ReadWrite
};

enum class ResourceType
{
	Texture2D,
	Buffer,
};

struct ResourceUsage
{
	std::string name;
	AccessType access;
	ResourceType type;
};

struct RenderPass
{
	std::string name;
	std::function<void(const FrameRenderContext&)> execute;

	std::vector<ResourceUsage> inputResources;
	std::vector<ResourceUsage> outputResources;

	//
	// TO-DO
		// add input/output/resources
};

class RenderGraph
{
public:
	void AddPass(const RenderPass& pass);
	void Compile();
	void Execute(const FrameRenderContext& context);

private:
	std::vector<RenderPass> m_passes;

	//
	// TO-DO
		// add compiled states, resource maps, execution order etc.
};