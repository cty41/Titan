// cache d11 state object make sure state objects will not recreate every frame, to do later -cty
#ifndef _D3D11_CACHE_STATES__HH
#define _D3D11_CACHE_STATES__HH

#include "D3D11Prerequisites.h"

namespace Titan
{
	//class _D3D11DllExport RasterizerState
	//{
	//public:
	//	RasterizerState()
	//	{}

	//	RasterizerState(const RasterizerState& state)
	//	{
	//		memcpy(&rasterizerState, &state.rasterizerState, sizeof(D3D11_RASTERIZER_DESC));
	//	}

	//	bool operator==(const RasterizerState& rhs)
	//	{
	//		return 0 == memcmp(&rasterizerState, &rhs.rasterizerState, sizeof(D3D11_RASTERIZER_DESC));
	//	}

	//	bool operator!=(const RasterizerState& rhs)
	//	{
	//		return 0 != memcmp(&rasterizerState, &rhs.rasterizerState, sizeof(D3D11_RASTERIZER_DESC));
	//	}


	//	D3D11_RASTERIZER_DESC rasterizerState;
	//};

#define DO_CACHE_D11STATES 0
}
#endif