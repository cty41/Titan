#ifndef _TITAN_D3D9VERTEXDECLARATION_HH
#define _TITAN_D3D9VERTEXDECLARATION_HH

#include "D3D9Prerequisites.h"
#include "TiVertexBuffer.h"

namespace Titan
{
	class _D3D9DllExport D3D9VertexDeclaration : public VertexDeclaration
	{
	public:
		D3D9VertexDeclaration();

		~D3D9VertexDeclaration();

		IDirect3DVertexDeclaration9*  getD3D9VertexDeclaration(); 

	protected:
		IDirect3DVertexDeclaration9*	mD3D9VertexDeclaration;
	};
}
#endif