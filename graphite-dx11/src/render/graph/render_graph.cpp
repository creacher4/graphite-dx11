#include "render_graph.h"

void RenderGraph::AddPass(const RenderPass& pass)
{
	m_passes.push_back(pass);
}

void RenderGraph::Execute(const FrameRenderContext& context)
{
	for (const auto& pass : m_passes)
	{
		pass.execute(context);
	}
}