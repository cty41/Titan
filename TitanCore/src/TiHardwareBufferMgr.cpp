#include "TitanStableHeader.h"
#include "TiHardwareBufferMgr.h"

namespace Titan
{
	template<> HardwareBufferMgr* Singleton<HardwareBufferMgr>::ms_Singleton = 0;

	HardwareBufferMgr* HardwareBufferMgr::getSingletonPtr()
	{
		return ms_Singleton;
	}
	HardwareBufferMgr& HardwareBufferMgr::getSingleton()
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}

	//-------------------------------------------------------------------------------//
	HardwareBufferMgr::HardwareBufferMgr()
	{

	}
	//-------------------------------------------------------------------------------//
	HardwareBufferMgr::~HardwareBufferMgr()
	{
		//why we do not delete , because we buffers are wrapped into the sharedPtr which means they will be deleted automatically
		mVertexBuffers.clear();
		mIndexBuffers.clear();

		destroyAllVertexBufferBindings();
		destroyAllVertexDeclarations();
	}
	//-------------------------------------------------------------------------------//
	VertexBufferBinding* HardwareBufferMgr::createVertexBufferBinding()
	{
		VertexBufferBinding* binding = TITAN_NEW VertexBufferBinding();
		mVertexBufferBindings.insert(binding);
		return binding;
	}
	//-------------------------------------------------------------------------------//
	void HardwareBufferMgr::destroyVertexDeclaration(VertexDeclaration* decl)
	{
		mVertexDeclarations.erase(decl);
		TITAN_DELETE decl;
	}
	//-------------------------------------------------------------------------------//
	void HardwareBufferMgr::destroyVertexBufferBinding(VertexBufferBinding* binding)
	{
		mVertexBufferBindings.erase(binding);
		TITAN_DELETE binding;
	}
	//-------------------------------------------------------------------------------//
	void HardwareBufferMgr::destroyAllVertexDeclarations()
	{
		VertexDeclarationList::iterator decl = mVertexDeclarations.begin(), iend = mVertexDeclarations.end();
		for (; decl != iend; ++decl)
		{
			TITAN_DELETE (*decl);
		}
		mVertexDeclarations.clear();
	}
	//-------------------------------------------------------------------------------//
	void HardwareBufferMgr::destroyAllVertexBufferBindings()
	{
		VertexBufferBindingList::iterator binding = mVertexBufferBindings.begin(), iend = mVertexBufferBindings.end();
		for(; binding != iend; ++binding)
		{
			TITAN_DELETE (*binding);
		}
		mVertexBufferBindings.clear();
	}
	//-------------------------------------------------------------------------------//
	void HardwareBufferMgr::_notifyVertexBufferDestroyed(VertexBuffer* buf)
	{
		VertexBufferList::iterator i = mVertexBuffers.find(buf);
		if (i != mVertexBuffers.end())
		{
			mVertexBuffers.erase(i);
		}
	}
	//-------------------------------------------------------------------------------//
	void HardwareBufferMgr::_notifyIndexBufferDestroyed(IndexBuffer* buf)
	{
		IndexBufferList::iterator i = mIndexBuffers.find(buf);
		if (i != mIndexBuffers.end())
		{
			mIndexBuffers.erase(i);
		}
	}
}
