#include "D3D9VertexDeclaration.h"
#include "D3D9Renderer.h"
#include "D3D9Mappings.h"


namespace Titan
{
	D3D9VertexDeclaration::D3D9VertexDeclaration()
		: mD3D9VertexDeclaration(0)
	{
	}
	//-------------------------------------------------------------//
	D3D9VertexDeclaration::~D3D9VertexDeclaration()
	{
		SAFE_RELEASE(mD3D9VertexDeclaration);
	}
	//-------------------------------------------------------------//
	IDirect3DVertexDeclaration9* D3D9VertexDeclaration::getD3D9VertexDeclaration()
	{
		if(!mD3D9VertexDeclaration)
		{
			D3DVERTEXELEMENT9* d3delems = new D3DVERTEXELEMENT9[mElementList.size() + 1];

			IDirect3DDevice9* pD3D9Device = D3D9Renderer::getSingleton().__getD3D9Device();

			VertexElementList::const_iterator i, iend;
			uint idx;
			iend = mElementList.end();
			for(i = mElementList.begin(), idx = 0; i != iend; ++i, ++idx)
			{
				d3delems[idx].Method = D3DDECLMETHOD_DEFAULT;
				d3delems[idx].Offset = static_cast<WORD>(i->getOffset());
				d3delems[idx].Stream = i->getSource();
				d3delems[idx].Type = D3D9Mappings::convertToD3D9(i->getType());
				d3delems[idx].Usage = D3D9Mappings::convertToD3D9(i->getSemantic());
				// NB force index if colours since D3D uses the same usage for 
				// diffuse & specular
				if (i->getSemantic() == VES_SPECULAR)
				{
					d3delems[idx].UsageIndex = 1;
				}
				else if (i->getSemantic() == VES_DIFFUSE)
				{
					d3delems[idx].UsageIndex = 0;
				}
				else
				{
					d3delems[idx].UsageIndex = static_cast<BYTE>(i->getIndex());
				}
			}
			// Add terminator
			d3delems[idx].Stream = 0xff;
			d3delems[idx].Offset = 0;
			d3delems[idx].Type = D3DDECLTYPE_UNUSED;
			d3delems[idx].Method = 0;
			d3delems[idx].Usage = 0;
			d3delems[idx].UsageIndex = 0;

			HRESULT hr = pD3D9Device->CreateVertexDeclaration(d3delems, &mD3D9VertexDeclaration);
			if(FAILED(hr))
			{
				TITAN_EXCEPT(Exception::EXCEP_RENDERAPI_ERROR, 
					"D3D9 Create IDirect3DVertexDeclaration9 failed" +
					D3D9Renderer::getSingleton().getErrorDiscription(hr), 
					"D3D9VertexDeclaration::getD3D9VertexDeclaration()");
			}

		}
		return mD3D9VertexDeclaration;
	}
}