#pragma once

#include <vector>
#include <functional>
#include <string>
#include "render/context/frame_render_context.h"

struct RenderPass
{
	std::string name;
	std::function<void(const FrameRenderContext&)> execute;

	//
	// TO-DO
		// add input/output/resources
};

class RenderGraph
{
public:
	void AddPass(const RenderPass& pass);
	void Execute(const FrameRenderContext& context);

private:
	std::vector<RenderPass> m_passes;
};