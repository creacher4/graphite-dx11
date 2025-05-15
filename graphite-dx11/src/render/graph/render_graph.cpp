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

void RenderGraph::RegisterExternalResource(const std::string& name, ID3D11RenderTargetView* rtv, ID3D11ShaderResourceView* srv)
{
	m_externalResources[name] = { rtv, srv };
}

ExternalResource RenderGraph::GetExternalResource(const std::string& name) const
{
	auto it = m_externalResources.find(name);
	if (it != m_externalResources.end()) return it->second;
	return {};
}