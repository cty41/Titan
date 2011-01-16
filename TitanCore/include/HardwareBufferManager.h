#ifndef _TITAN_HARDWAREBUFFERMANAGER_HH
#define _TITAN_HARDWAREBUFFERMANAGER_HH

#include "TitanPrerequisites.h"
#include "Singleton.h"
#include "VertexBuffer.h"

namespace Titan
{
	class _DllExport HardwareBufferManager : public Singleton<HardwareBufferManager>, public GeneralAlloc
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
		HardwareBufferManager();

		virtual ~HardwareBufferManager();

		virtual VertexBufferSharedPtr	createVertexBuffer(
			size_t vertexSize, size_t numVertices,
			VertexBuffer::Usage usage, bool useSystemMemory) = 0;

		virtual IndexBufferSharedPtr	createIndexBuffer(
			size_t numIndexes, HardwareBuffer::Usage usage,
			bool useSystemMemory) = 0;

		virtual VertexDeclaration*		createVertexDeclaration();

		VertexBufferBinding*			createVertexBufferBinding();

		virtual void					destroyVertexDeclaration(VertexDeclaration*	decl);

		void							destroyVertexBufferBinding(VertexBufferBinding* binding);

		void							destroyAllVertexDeclarations();

		void							destroyAllVertexBufferBindings();

		void							_notifyVertexBufferDestroyed(VertexBuffer* buf);

		void							_notifyIndexBufferDestroyed(IndexBuffer* buf);

		static	HardwareBufferManager*	getSingletonPtr();

		static  HardwareBufferManager&	getSingleton();

	};

}
#endif