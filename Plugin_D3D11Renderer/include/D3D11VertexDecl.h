#ifndef __D3D11_VERTEX_DECL__HH
#define __D3D11_VERTEX_DECL__HH

#include "D3D11Prerequisites.h"
#include "TiVertexBuffer.h"


namespace Titan
{
#define D11_MAX_VERTEX_ELEMENT_NUM 16
	class _D3D11DllExport D3D11VertexDecl : public VertexDeclaration
	{
	public:
		typedef hash_map<D3D11HLSLShader*, ID3D11InputLayout*>::type ShaderToLayoutMap;
	public:
		D3D11VertexDecl();

		~D3D11VertexDecl();

		ID3D11InputLayout*	getD11InputLayoutByShader(D3D11HLSLShader* shader);

		void		createInputElementDescs();
	protected:
		ShaderToLayoutMap	mShaderToLayoutMap;

		D3D11_INPUT_ELEMENT_DESC	mInputElementDescs[D11_MAX_VERTEX_ELEMENT_NUM];
		/// the input layout need to recreate?
		bool mNeedRecreate;
	};
}
#endif