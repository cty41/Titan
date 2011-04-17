#ifndef _TITAN_HARDWAREBUFFERMANAGER_HH
#define _TITAN_HARDWAREBUFFERMANAGER_HH

#include "TiPrerequisites.h"
#include "TiSingleton.h"
#include "TiVertexBuffer.h"

namespace Titan
{
	class _DllExport HardwareBufferMgr : public Singleton<HardwareBufferMgr>, public GeneralAlloc
	{
		friend class VertexBufferSharedPtr;
		friend class IndexBufferSharedPtr;
	protected:
		typedef set<VertexBuffer*>::type VertexBufferList;
		typedef set<IndexBuffer*>::type IndexBufferList;
		VertexBufferList mVertexBuffers;
		IndexBufferList mIndexBuffers;


		typedef set<VertexDeclaration*>::type VertexDeclarationList;
		typedef set<VertexBufferBinding*>::type VertexBufferBindingList;
		VertexDeclarationList mVertexDeclarations;
		VertexBufferBindingList mVertexBufferBindings;
	public:
		HardwareBufferMgr();

		virtual ~HardwareBufferMgr();

		virtual VertexBufferSharedPtr	createVertexBuffer(
			size_t vertexSize, size_t numVertices,
			VertexBuffer::Usage usage, bool useSystemMemory = false) = 0;

		virtual IndexBufferSharedPtr	createIndexBuffer(
			size_t numIndexes, HardwareBuffer::Usage usage,
			bool useSystemMemory = false) = 0;

		virtual VertexDeclaration*		createVertexDeclaration() = 0;

		VertexBufferBinding*			createVertexBufferBinding();

		virtual void					destroyVertexDeclaration(VertexDeclaration*	decl);

		void							destroyVertexBufferBinding(VertexBufferBinding* binding);

		void							destroyAllVertexDeclarations();

		void							destroyAllVertexBufferBindings();

		void							_notifyVertexBufferDestroyed(VertexBuffer* buf);

		void							_notifyIndexBufferDestroyed(IndexBuffer* buf);

		static	HardwareBufferMgr*	getSingletonPtr();

		static  HardwareBufferMgr&	getSingleton();

	};

}
#endif