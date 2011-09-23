#ifndef _TITAN_D3D11MAPPINGS_H
#define _TITAN_D3D11MAPPINGS_H

#include "D3D11Prerequisites.h"
#include "TiCommon.h"
#include "TiHardwareBuffer.h"
#include "TiTexture.h"
#include "TiShaderParams.h"
#include "TitanBlendMode.h"
#include "TiTextureUnit.h"
#include "TiRenderData.h"

namespace Titan
{
	class _D3D11DllExport D3D11Mappings
	{
	public:

		static D3D11_CULL_MODE convertToD3D11(CullingMode cm);

		static D3D11_FILL_MODE convertToD3D11(PolygonMode level);

		static D3D11_BLEND convertToD3D11(SceneBlendFactor sbf);

		static D3D11_BLEND_OP convertToD3D11(SceneBlendOperation sbo);

		static D3D11_TEXTURE_ADDRESS_MODE convertToD3D11(TexAddressMode tam);

		static D3D11_FILTER  convertToD3D11(FilterOptions fo);

		static D3D11_COMPARISON_FUNC convertToD3D11(CompareFunction cf);

		static D3D11_USAGE convertToD3D11(HardwareBuffer::Usage usage);

		static D3D11_MAP convertToD3D11(HardwareBuffer::LockOptions options, HardwareBuffer::Usage usage);

		static LPCSTR convertToD3D11(VertexElementSemantic sem);

		static DXGI_FORMAT convertToD3D11(VertexElementType vType);

		static D3D10_PRIMITIVE_TOPOLOGY convertToD3D11(OperationType ot);

		static DXGI_FORMAT  convertToD3D11(IndexBufferType ibt);
	};
}
#endif