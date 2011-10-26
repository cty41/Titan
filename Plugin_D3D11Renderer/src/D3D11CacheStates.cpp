#include "D3D11CacheStates.h"

namespace Titan
{
	bool RasterizerState::operator==(const RasterizerState& rhs)
	{
		return 0 == memcmp(&desc, &rhs.desc, sizeof(D3D11_RASTERIZER_DESC));
	}

	bool RasterizerState::operator!=(const RasterizerState& rhs)
	{
		return 0 != memcmp(&desc, &rhs.desc, sizeof(D3D11_RASTERIZER_DESC));
	}

	bool RasterizerState::operator<(const RasterizerState& rhs) const
	{
		if(sortKey < rhs.sortKey)
			return true;
		else if(sortKey > rhs.sortKey)
			return false;
		else
		{
			if (desc.DepthBias < rhs.desc.DepthBias)
				return true;
			else if(desc.DepthBias > rhs.desc.DepthBias)
				return false;
			else
			{
				if (desc.DepthBiasClamp  < rhs.desc.DepthBiasClamp )
					return true;
				else if(desc.DepthBiasClamp > rhs.desc.DepthBiasClamp)
					return false;
				else
				{
					if (desc.SlopeScaledDepthBias < rhs.desc.SlopeScaledDepthBias)
						return true;
				}
			}
		}
		return false;
	}

	bool RasterizerState::operator>(const RasterizerState& rhs) const
	{
		if(sortKey > rhs.sortKey)
			return true;
		else if(sortKey < rhs.sortKey)
			return false;
		else
		{
			if (desc.DepthBias > rhs.desc.DepthBias)
				return true;
			else if(desc.DepthBias < rhs.desc.DepthBias)
				return false;
			else
			{
				if (desc.DepthBiasClamp  > rhs.desc.DepthBiasClamp )
					return true;
				else if(desc.DepthBiasClamp < rhs.desc.DepthBiasClamp)
					return false;
				else
				{
					if (desc.SlopeScaledDepthBias > rhs.desc.SlopeScaledDepthBias)
						return true;
				}
			}
		}
		return false;
	}

	void RasterizerState::makeSortKey()
	{
		sortKey = 0;

		sortKey |= desc.CullMode; //2
		sortKey |= desc.FillMode <<2;//2
		if(desc.FrontCounterClockwise)
			sortKey |= 1 << 4; //1
		if (desc.DepthClipEnable)
			sortKey |= 1<<5;

		if(desc.ScissorEnable)
			sortKey |= 1<<6;

		if(desc.MultisampleEnable)
			sortKey |= 1<<7;

		if(desc.AntialiasedLineEnable)
			sortKey |= 1<<8;
	}



	bool SamplerState::operator==(const SamplerState& rhs)
	{
		return 0 == memcmp(&desc, &rhs.desc, sizeof(D3D11_SAMPLER_DESC));
	}

	bool SamplerState::operator!=(const SamplerState& rhs)
	{
		return 0 != memcmp(&desc, &rhs.desc, sizeof(D3D11_SAMPLER_DESC));
	}

	bool SamplerState::operator<(const SamplerState& rhs) const
	{
		if(sortKey < rhs.sortKey)
			return true;
		else if(sortKey > rhs.sortKey)
			return false;
		else
		{
			if (desc.MipLODBias  < rhs.desc.MipLODBias )
				return true;
			else if(desc.MipLODBias  > rhs.desc.MipLODBias )
				return false;
			else
			{
				if (desc.MaxLOD  < rhs.desc.MaxLOD   )
					return true;
				else if(desc.MaxLOD > rhs.desc.MaxLOD )
					return false;
				else
				{
					if (desc.BorderColor  < rhs.desc.BorderColor )
						return true;
				}
			}
		}
		return false;
	}

	bool SamplerState::operator>(const SamplerState& rhs) const
	{
		if(sortKey > rhs.sortKey)
			return true;
		else if(sortKey < rhs.sortKey)
			return false;
		else
		{
			if (desc.MipLODBias > rhs.desc.MipLODBias )
				return true;
			else if(desc.MipLODBias < rhs.desc.MipLODBias )
				return false;
			else
			{
				if (desc.MaxLOD > rhs.desc.MaxLOD   )
					return true;
				else if(desc.MaxLOD < rhs.desc.MaxLOD )
					return false;
				else
				{
					if (desc.BorderColor > rhs.desc.BorderColor )
						return true;
				}
			}
		}
		return false;
	}

	void SamplerState::makeSortKey()
	{
		sortKey = 0;

		sortKey |= desc.Filter; 
		sortKey |= desc.AddressU << 8;//3
		sortKey |= desc.AddressV << 11;//3
		sortKey |= desc.AddressW << 14;//3
		sortKey |= desc.ComparisonFunc << 17;//3
	}




	bool BlendState::operator==(const BlendState& rhs)
	{
		return 0 == memcmp(&desc, &rhs.desc, sizeof(D3D11_BLEND_DESC));
	}

	bool BlendState::operator!=(const BlendState& rhs)
	{
		return 0 != memcmp(&desc, &rhs.desc, sizeof(D3D11_BLEND_DESC));
	}

	bool BlendState::operator<(const BlendState& rhs) const
	{
		if(sortKey < rhs.sortKey)
			return true;
		else if(sortKey > rhs.sortKey)
			return false;
		else
		{
			if (desc.RenderTarget[0].RenderTargetWriteMask < rhs.desc.RenderTarget[0].RenderTargetWriteMask )
				return true;
		}
		return false;
	}

	bool BlendState::operator>(const BlendState& rhs) const
	{
		if(sortKey > rhs.sortKey)
			return true;
		else if(sortKey < rhs.sortKey)
			return false;
		else
		{
			if (desc.RenderTarget[0].RenderTargetWriteMask > rhs.desc.RenderTarget[0].RenderTargetWriteMask )
				return true;
		}
		return false;
	}

	void BlendState::makeSortKey()
	{
		sortKey = 0;

		sortKey |= desc.AlphaToCoverageEnable; 
		sortKey |= desc.IndependentBlendEnable << 1;
		sortKey |= desc.RenderTarget[0].BlendEnable << 2;
		sortKey |= desc.RenderTarget[0].SrcBlend<< 3;
		sortKey |= desc.RenderTarget[0].DestBlend<< 8;
		sortKey |= desc.RenderTarget[0].BlendOp<< 13;
		sortKey |= desc.RenderTarget[0].SrcBlendAlpha<< 16;
		sortKey |= desc.RenderTarget[0].DestBlendAlpha<< 21;
		sortKey |= desc.RenderTarget[0].BlendOpAlpha << 26;
	}



	bool DepthStencilState::operator==(const DepthStencilState& rhs)
	{
		return 0 == memcmp(&desc, &rhs.desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	}

	bool DepthStencilState::operator!=(const DepthStencilState& rhs)
	{
		return 0 != memcmp(&desc, &rhs.desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	}

	bool DepthStencilState::operator<(const DepthStencilState& rhs) const
	{
		if(sortKey < rhs.sortKey)
			return true;
		else if(sortKey > rhs.sortKey)
			return false;
		else
		{
			if (desc.StencilReadMask < rhs.desc.StencilReadMask)
				return true;
			else if(desc.StencilReadMask > rhs.desc.StencilReadMask )
				return false;
			else
			{
				if (desc.StencilWriteMask  < rhs.desc.StencilWriteMask )
					return true;
			}
		}
		return false;
	}

	bool DepthStencilState::operator>(const DepthStencilState& rhs) const
	{
		if(sortKey > rhs.sortKey)
			return true;
		else if(sortKey < rhs.sortKey)
			return false;
		else
		{
			if (desc.StencilReadMask  > rhs.desc.StencilReadMask)
				return true;
			else if(desc.StencilReadMask < rhs.desc.StencilReadMask )
				return false;
			else
			{
				if (desc.StencilWriteMask  > rhs.desc.StencilWriteMask )
					return true;
			}
		}
		return false;
	}

	void DepthStencilState::makeSortKey()
	{
		sortKey = 0;

		sortKey |= desc.DepthEnable; 
		sortKey |= desc.DepthWriteMask << 1;
		sortKey |= desc.DepthFunc << 2;
		sortKey |= desc.StencilEnable << 5;
		sortKey |= (desc.FrontFace.StencilDepthFailOp - 1) << 6;
		sortKey |= (desc.FrontFace.StencilFailOp - 1) << 9;
		sortKey |= (desc.FrontFace.StencilFunc - 1) << 12;
		sortKey |= (desc.FrontFace.StencilPassOp - 1) << 15;
		sortKey |= (desc.BackFace.StencilDepthFailOp - 1) << 18;
		sortKey |= (desc.BackFace.StencilFailOp - 1) << 21;
		sortKey |= (desc.BackFace.StencilFunc - 1) << 24;
		sortKey |= (desc.BackFace.StencilPassOp - 1) << 27;
	}

}