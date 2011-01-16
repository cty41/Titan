#include "TitanStableHeader.h"
#include "HardwareBufferManager.h"

namespace Titan
{
	template<> HardwareBufferManager* Singleton<HardwareBufferManager>::ms_Singleton = 0;

	HardwareBufferManager* HardwareBufferManager::getSingletonPtr()
	{
		return ms_Singleton;
	}
	HardwareBufferManager& HardwareBufferManager::getSingleton()
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}

	//-------------------------------------------------------------//
	HardwareBufferManager::HardwareBufferManager()
	{

	}
	//-------------------------------------------------------------//
	HardwareBufferManager::~HardwareBufferManager()
	{
		mVertexBuffers.clear();
		for(IndexBufferList::iterator i = mIndexBuffers.begin(); i != mIndexBuffers.end();)
		{


			IndexBuffer* buffer = (*i);
			TITAN_DELETE buffer;
						mIndexBuffers.erase(i++);
		}

		mIndexBuffers.clear();

		destroyAllVertexBufferBindings();
		destroyAllVertexDeclarations();
	}
	//-------------------------------------------------------------//
	VertexDeclaration*	HardwareBufferManager::createVertexDeclaration()
	{
		VertexDeclaration* decl = TITAN_NEW VertexDeclaration();
		mVertexDeclarations.insert(decl);
		return decl;
	}
	//-------------------------------------------------------------//
	VertexBufferBinding* HardwareBufferManager::createVertexBufferBinding()
	{
		VertexBufferBinding* binding = TITAN_NEW VertexBufferBinding();
		mVertexBufferBindings.insert(binding);
		return binding;
	}
	//-------------------------------------------------------------//
	void HardwareBufferManager::destroyVertexDeclaration(VertexDeclaration* decl)
	{
		mVertexDeclarations.erase(decl);
		TITAN_DELETE decl;
	}
	//-------------------------------------------------------------//
	void HardwareBufferManager::destroyVertexBufferBinding(VertexBufferBinding* binding)
	{
		mVertexBufferBindings.erase(binding);
		TITAN_DELETE binding;
	}
	//-------------------------------------------------------------//
	void HardwareBufferManager::destroyAllVertexDeclarations()
	{
		VertexDeclarationList::iterator decl = mVertexDeclarations.begin(), iend = mVertexDeclarations.end();
		for (; decl != iend; ++decl)
		{
			TITAN_DELETE (*decl);
		}
		mVertexDeclarations.clear();
	}
	//-------------------------------------------------------------//
	void HardwareBufferManager::destroyAllVertexBufferBindings()
	{
		VertexBufferBindingList::iterator binding = mVertexBufferBindings.begin(), iend = mVertexBufferBindings.end();
		for(; binding != iend; ++binding)
		{
			TITAN_DELETE (*binding);
		}
		mVertexBufferBindings.clear();
	}
	//-------------------------------------------------------------//
	void HardwareBufferManager::_notifyVertexBufferDestroyed(VertexBuffer* buf)
	{
		VertexBufferList::iterator i = mVertexBuffers.find(buf);
		if (i != mVertexBuffers.end())
		{
			mVertexBuffers.erase(i);
		}
	}
	//-------------------------------------------------------------//
	void HardwareBufferManager::_notifyIndexBufferDestroyed(IndexBuffer* buf)
	{
		IndexBufferList::iterator i = mIndexBuffers.find(buf);
		if (i != mIndexBuffers.end())
		{
			mIndexBuffers.erase(i);
		}
	}
}
