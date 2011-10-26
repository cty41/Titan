// cache d11 state object make sure state objects will not recreate every frame, to do later -cty
#ifndef _D3D11_CACHE_STATES__HH
#define _D3D11_CACHE_STATES__HH

#include "D3D11Prerequisites.h"

namespace Titan
{
	class  RasterizerState
	{
	public:
		RasterizerState()
		{}

		RasterizerState(const D3D11_RASTERIZER_DESC& desc)
			: desc(desc)
		{
			makeSortKey();
		}

		RasterizerState(const RasterizerState& state)
		{
			memcpy(&desc, &state.desc, sizeof(D3D11_RASTERIZER_DESC));
			sortKey = state.sortKey;
		}

		bool operator==(const RasterizerState& rhs);

		bool operator!=(const RasterizerState& rhs);

		bool operator<(const RasterizerState& rhs) const;

		bool operator>(const RasterizerState& rhs) const;

		void makeSortKey();


		D3D11_RASTERIZER_DESC desc;
		size_t	sortKey;
	};

	class  SamplerState
	{
	public:
		SamplerState()
		{}

		SamplerState(const D3D11_SAMPLER_DESC& desc)
			: desc(desc)
		{
			makeSortKey();
		}

		SamplerState(const SamplerState& state)
		{
			memcpy(&desc, &state.desc, sizeof(D3D11_SAMPLER_DESC));
			sortKey = state.sortKey;
		}

		bool operator==(const SamplerState& rhs);

		bool operator!=(const SamplerState& rhs);

		bool operator<(const SamplerState& rhs) const;

		bool operator>(const SamplerState& rhs) const;

		void makeSortKey();


		D3D11_SAMPLER_DESC desc;
		size_t	sortKey;
	};

	class  BlendState
	{
	public:
		BlendState()
		{}

		BlendState(const D3D11_BLEND_DESC& desc)
			: desc(desc)
		{
			makeSortKey();
		}

		BlendState(const BlendState& state)
		{
			memcpy(&desc, &state.desc, sizeof(D3D11_BLEND_DESC));
			sortKey = state.sortKey;
		}

		bool operator==(const BlendState& rhs);

		bool operator!=(const BlendState& rhs);

		bool operator<(const BlendState& rhs) const;

		bool operator>(const BlendState& rhs) const;

		void makeSortKey();


		D3D11_BLEND_DESC desc;
		size_t	sortKey;
	};

	class  DepthStencilState
	{
	public:
		DepthStencilState()
		{}

		DepthStencilState(const D3D11_DEPTH_STENCIL_DESC& desc)
			: desc(desc)
		{
			makeSortKey();
		}

		DepthStencilState(const DepthStencilState& state)
		{
			memcpy(&desc, &state.desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
			sortKey = state.sortKey;
		}

		bool operator==(const DepthStencilState& rhs);

		bool operator!=(const DepthStencilState& rhs);

		bool operator<(const DepthStencilState& rhs) const;

		bool operator>(const DepthStencilState& rhs) const;

		void makeSortKey();

		D3D11_DEPTH_STENCIL_DESC desc;
		size_t	sortKey;
	};

#define DO_CACHE_D11STATES 1
}
#endif