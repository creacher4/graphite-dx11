#pragma once

#include <d3d11.h>
#include <vector>
#include <functional>
#include <string>
#include "render/context/frame_render_context.h"

struct ExternalResource
{
	ID3D11RenderTargetView* rtv = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
};

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

struct RenderPassDesc
{
	std::string name;
	std::function<void(const FrameRenderContext&)> execute;
	std::vector<ResourceUsage> inputResources;
	std::vector<ResourceUsage> outputResources;

	// future metadata
};

class RenderGraph
{
public:
	void AddPass(const RenderPassDesc& pass);
	void Compile();
	void Execute(const FrameRenderContext& context);

	void ImportResource(const std::string& name, ID3D11RenderTargetView* rtv, ID3D11ShaderResourceView* srv);


	ExternalResource GetExternalResource(const std::string& name) const;
	

private:
	std::vector<RenderPassDesc> m_passes;
	std::unordered_map<std::string, ExternalResource> m_externalResources;

	//
	// TO-DO
		// add compiled states, resource maps, execution order etc.
};