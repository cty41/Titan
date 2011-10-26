#include "TitanStableHeader.h"
#include "TiManualObject.h"
#include "TiHardwareBufferMgr.h"

#include "TiRenderQueue.h"
#include "TiMaterialMgr.h"

namespace Titan
{
	static const TexturePtr sNullTexPtr;

#define TEMP_INITIAL_SIZE 50
#define TEMP_VERTEXSIZE_GUESS sizeof(float) * 12
#define TEMP_INITIAL_VERTEX_SIZE TEMP_VERTEXSIZE_GUESS * TEMP_INITIAL_SIZE
#define TEMP_INITIAL_INDEX_SIZE sizeof(uint32) * TEMP_INITIAL_SIZE

	//-------------------------------------------------------------------------------//
	ManualObject::ManualObject(SceneObjectFactory* creator, const String& name)
		: SceneObject(creator, name), mFirstVertex(true),
		mTempVertexPending(false), mCurrentSection(0),
		mTempVertexBuffer(0),mTempVertexSize(TEMP_INITIAL_VERTEX_SIZE),
		mTempIndexBuffer(0), mTempIndexSize(TEMP_INITIAL_INDEX_SIZE),
		mDeclSize(0), mTexCoordIndex(0)
	{
		//If you got an error here, recompiled it..
	}
	//-------------------------------------------------------------------------------//
	ManualObject::~ManualObject()
	{
		clear();
	}
	//-------------------------------------------------------------------------------//
	void ManualObject::_updateRenderQueue(RenderQueue* queue, Camera* cam)
	{
		ManualObjectSectionVector::iterator it = mSectionVector.begin(),
			itend = mSectionVector.end();
		while(it != itend)
		{
			queue->addRenderable(*it, mRenderQueueID, mRenderQueuePriority);
			++it;
		}
	}
	//------------------------------------------------------------------------------//
	void ManualObject::setMaterial(const String& name, const String& group)
	{
		if(!mCurrentSection)
		{
			TITAN_EXCEPT_INTERNALERROR("you forgot to call begin() before you set material");
		}
		mCurrentSection->setMaterial(name, group);
	}
	//-------------------------------------------------------------------------------//
	void ManualObject::begin(OperationType ot)
	{
		if(mCurrentSection)
		{
			TITAN_EXCEPT_INTERNALERROR("you forgot to call end() to finish last Manual operation");
		}
		mCurrentSection = TITAN_NEW ManualObjectSection(this, ot);
		mSectionVector.push_back(mCurrentSection);

		mFirstVertex = true;
		mDeclSize = 0;
		mTexCoordIndex = 0;
	}
	//-------------------------------------------------------------------------------//
	ManualObject::ManualObjectSection* ManualObject::end()
	{
		if(!mCurrentSection)
		{ 
			TITAN_EXCEPT_INTERNALERROR("You must call begin() before end()");
		}

		if (mTempVertexPending)
		{
			// bake current vertex
			copyTempVertexToBuffer();
		}

		ManualObjectSection* pSection = mCurrentSection;
		RenderData* rd = mCurrentSection->getRenderData();
		if(rd->vertexData->vertexCount == 0)
		{
			mSectionVector.pop_back();
			TITAN_DELETE mCurrentSection;
		}
		else
		{
			VertexBufferPtr vbuf;

			vbuf = HardwareBufferMgr::getSingleton().createVertexBuffer(
				mDeclSize, rd->vertexData->vertexCount,
				HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);

			rd->vertexData->vertexBufferBinding->setBinding(0, vbuf);

			vbuf->writeData(
				0, rd->vertexData->vertexCount * mDeclSize,
				mTempVertexBuffer, true);
	
			if(rd->useIndex)		//16bit index, expand later
			{
				rd->indexData->indexBuffer = HardwareBufferMgr::getSingleton().createIndexBuffer(
					rd->indexData->indexCount,
					HardwareBuffer::HBU_STATIC_WRITE_ONLY, true);

				rd->indexData->indexBuffer->writeData(0, rd->indexData->indexCount * rd->indexData->indexBuffer->getIndexSize(), (void*)mTempIndexBuffer, true);

			}
		}

		mCurrentSection = 0;
		return pSection;
	}
	//-------------------------------------------------------------------------------//
	void ManualObject::clear()
	{
		mAABB.setNull();
		resetTempAreas();
		ManualObjectSectionVector::iterator it = mSectionVector.begin(), itend = mSectionVector.end();

		while(it != itend)
		{
			TITAN_DELETE *it;
			++it;
		}

	}
	//-------------------------------------------------------------------------------//
	void ManualObject::resetTempAreas(void)
	{
		TITAN_FREE(mTempVertexBuffer, MEMCATEGORY_GENERAL);
		TITAN_FREE(mTempIndexBuffer, MEMCATEGORY_GENERAL);
		mTempVertexBuffer = 0;
		mTempIndexBuffer = 0;
		mTempVertexSize = TEMP_INITIAL_VERTEX_SIZE;
		mTempIndexSize = TEMP_INITIAL_INDEX_SIZE;
	}
	//-------------------------------------------------------------------------------//
	void ManualObject::position(float x, float y, float z)
	{
		if(mCurrentSection == NULL)
		{
			TITAN_EXCEPT_INTERNALERROR("Must call begin first");
		}

		if (mTempVertexPending)
		{
			// bake current vertex
			copyTempVertexToBuffer();
			mFirstVertex = false;
		}
		
		RenderData* rd = mCurrentSection->getRenderData();
		if (mFirstVertex )
		{
			// defining declaration
			rd->vertexData->vertexDecl
				->addElement(0, mDeclSize, VET_FLOAT3, VES_POSITION);
			mDeclSize += VertexElement::getTypeSize(VET_FLOAT3);
		}

		mTempVertex.position.x = x;
		mTempVertex.position.y = y;
		mTempVertex.position.z = z;

		mAABB.merge(mTempVertex.position);

		mTexCoordIndex = 0;

		mTempVertexPending = true;
	}
	//-------------------------------------------------------------------------------//
	void ManualObject::position(const Vector3& pos)
	{
		position(pos.x, pos.y, pos.z);
	}
	//------------------------------------------------------------------------------//
	void ManualObject::texCoord(float u, float v)
	{
		if(mCurrentSection == NULL)
		{
			TITAN_EXCEPT_INTERNALERROR("Must call begin first");
		}
		if (mFirstVertex)
		{
			RenderData* rd = mCurrentSection->getRenderData();
			rd->vertexData->vertexDecl->addElement(0, mDeclSize, VET_FLOAT2,
				VES_TEXTURE_COORDINATES, mTexCoordIndex);
			mDeclSize += VertexElement::getTypeSize(VET_FLOAT2);
		}

		mTempVertex.texCoordDims[mTexCoordIndex] = 2;
		mTempVertex.texCoords[mTexCoordIndex].x = u;
		mTempVertex.texCoords[mTexCoordIndex].y = v;
		++mTexCoordIndex;
	}
	//------------------------------------------------------------------------------//
	void ManualObject::texCoord(const Vector2& uv)
	{
		texCoord(uv.x, uv.y);
	}
	//------------------------------------------------------------------------------//
	void ManualObject::texCoord(float u, float v, float w)
	{
		if(mCurrentSection == NULL)
		{
			TITAN_EXCEPT_INTERNALERROR("Must call begin first");
		}
		if (mFirstVertex)
		{
			RenderData* rd = mCurrentSection->getRenderData();
			rd->vertexData->vertexDecl->addElement(0, mDeclSize, VET_FLOAT3,
				VES_TEXTURE_COORDINATES, mTexCoordIndex);
			mDeclSize += VertexElement::getTypeSize(VET_FLOAT3);
		}

		mTempVertex.texCoordDims[mTexCoordIndex] = 3;
		mTempVertex.texCoords[mTexCoordIndex].x = u;
		mTempVertex.texCoords[mTexCoordIndex].y = v;
		mTempVertex.texCoords[mTexCoordIndex].z = w;
		++mTexCoordIndex;
	}
	//------------------------------------------------------------------------------//
	void ManualObject::texCoord(const Vector3& uvw)
	{
		texCoord(uvw.x, uvw.y, uvw.z);
	}
	//-------------------------------------------------------------------------------//
	void ManualObject::index(uint idx)
	{
		RenderData* rd = mCurrentSection->getRenderData();
		if(rd->indexData == 0)
		{
			rd->indexData = TITAN_NEW IndexData();
			rd->indexData->indexCount = 0;
		}
		rd->useIndex = true;
		resizeTempIndexBufferIfNeeded(++rd->indexData->indexCount);

		mTempIndexBuffer[rd->indexData->indexCount - 1] = idx;
	}
	//-------------------------------------------------------------------------------//
	void ManualObject::triangle(uint32 i1, uint32 i2, uint32 i3)
	{
		RenderData* rd = mCurrentSection->getRenderData();
		if (rd->operationType != OT_TRIANGLE_LIST)
		{
			TITAN_EXCEPT_INVALIDPARAMS("This method is only valid on triangle lists");
		}

		index(i1);
		index(i2);
		index(i3);
	}
	//-------------------------------------------------------------------------------//
	void ManualObject::quad(uint32 i1, uint32 i2, uint32 i3, uint32 i4)
	{
		// first tri
		triangle(i1, i2, i3);
		// second tri
		triangle(i3, i4, i1);
	}
	//-------------------------------------------------------------------------------//
	void ManualObject::copyTempVertexToBuffer()
	{

		mTempVertexPending = false;

		RenderData* rd = mCurrentSection->getRenderData();

		resizeTempVertexBufferIfNeeded(++rd->vertexData->vertexCount);

		// get base pointer
		char* pBase = mTempVertexBuffer + (mDeclSize * (rd->vertexData->vertexCount-1));
		const VertexDeclaration::VertexElementList& elemList =
			rd->vertexData->vertexDecl->getElements();
		for (VertexDeclaration::VertexElementList::const_iterator i = elemList.begin();
			i != elemList.end(); ++i)
		{
			float* pFloat = 0;
			RGBA* pRGBA = 0;
			const VertexElement& elem = *i;
			switch(elem.getType())
			{
			case VET_FLOAT1:
			case VET_FLOAT2:
			case VET_FLOAT3:
			case VET_FLOAT4:
				elem.baseVertexPointerToElement(pBase, &pFloat);
				break;
			case VET_COLOR:
			case VET_COLOR_ABGR:
			case VET_COLOR_ARGB:
				elem.baseVertexPointerToElement(pBase, &pRGBA);
				break;
			default:
				// nop ?
				break;
			};


			unsigned short dims;

			switch(elem.getSemantic())
			{
			case VES_POSITION:
				*pFloat++ = mTempVertex.position.x;
				*pFloat++ = mTempVertex.position.y;
				*pFloat++ = mTempVertex.position.z;
				break;

			case VES_TEXTURE_COORDINATES:
				dims = VertexElement::getTypeCount(elem.getType());
				for (ushort t = 0; t < dims; ++t)
					*pFloat++ = mTempVertex.texCoords[elem.getIndex()][t];
				break;

			default:
				// nop ?
				break;
			};

		}
	}
	//-------------------------------------------------------------------------------//
	void ManualObject::resizeTempVertexBufferIfNeeded(size_t numVerts)
	{
		size_t newSize;
		if(!mFirstVertex)
		{
			newSize = mDeclSize * numVerts;
		}
		else
		{
			// estimate - size checks will deal for subsequent verts
			newSize = TEMP_VERTEXSIZE_GUESS * numVerts;
		}
		if (newSize > mTempVertexSize || !mTempVertexBuffer)
		{
			if (!mTempVertexBuffer)
			{
				// init
				newSize = mTempVertexSize;
			}
			else
			{
				// increase to at least double current
				newSize = std::max(newSize, mTempVertexSize*2);
			}
			// copy old data
			char* tmp = mTempVertexBuffer;
			mTempVertexBuffer = TITAN_ALLOC_T(char, newSize, MEMCATEGORY_GENERAL);
			if (tmp)
			{
				memcpy(mTempVertexBuffer, tmp, mTempVertexSize);
				// delete old buffer
				TITAN_FREE(tmp, MEMCATEGORY_GENERAL);
			}
			mTempVertexSize = newSize;
		}
	}
	//-------------------------------------------------------------------------------//
	void ManualObject::resizeTempIndexBufferIfNeeded(size_t numInds)
	{
		size_t newSize = numInds * sizeof(uint16);
		if (newSize > mTempIndexSize || !mTempIndexBuffer)
		{
			if (!mTempIndexBuffer)
			{
				// init
				newSize = mTempIndexSize;
			}
			else
			{
				// increase to at least double current
				newSize = std::max(newSize, mTempIndexSize*2);
			}
			numInds = newSize / sizeof(uint16);
			uint16* tmp = mTempIndexBuffer;
			mTempIndexBuffer = TITAN_ALLOC_T(uint16, numInds, MEMCATEGORY_GENERAL);
			if (tmp)
			{
				memcpy(mTempIndexBuffer, tmp, mTempIndexSize);
				TITAN_FREE(tmp, MEMCATEGORY_GENERAL);
			}
			mTempIndexSize = newSize;
		}
	}

	typedef ManualObject::ManualObjectSection ManualObjectSection;

	ManualObjectSection::ManualObjectSection(ManualObject* parent, OperationType ot)
		:mParent(parent)
	{
		mRenderData.operationType = ot;
		mRenderData.useIndex = false;
		mRenderData.vertexData = TITAN_NEW VertexData();

		//mMaterial = MaterialMgr::getSingleton().getDefaultMaterial();
	}
	//-------------------------------------------------------------------------------//
	ManualObjectSection::~ManualObjectSection()
	{
	}	
	//-------------------------------------------------------------------------------//
	void ManualObjectSection::setMaterial(const String& name, const String&  group)
	{
		if(mMaterialName != name || mMaterialGroup != group)
		{
			mMaterialName = name;
			mMaterialGroup = group;
			mMaterial.setNull();
			mMaterial = MaterialMgr::getSingleton().load(mMaterialName, mMaterialGroup);
		}
	}
	//------------------------------------------------------------------------------//
	void ManualObjectSection::getRenderData(RenderData& rd)
	{
		rd = mRenderData;
	}
	//-------------------------------------------------------------------------------//
	void ManualObjectSection::getTransformMat(Matrix4* transMat)
	{
		*transMat = mParent->_getAttachedNodeFullTransform();
	}
	//----------------------------------------------------------------------------//
	inline const MaterialPtr& ManualObjectSection::getMaterial() const
	{
		return mMaterial;
	}
	//-------------------------------------------------------------------------------//
	float ManualObjectSection::getSquaredDistance(Camera* cam)
	{
		return mParent->getAttachedNode()->getSquaredDistance(cam);
	}
	

	//
	//ManualObject Factory
	//
	String ManualObjectFactory::mType = "ManualObject";
	ManualObjectFactory::ManualObjectFactory()
	{

	}
	//-------------------------------------------------------------------------------//
	ManualObjectFactory::~ManualObjectFactory()
	{

	}
	//-------------------------------------------------------------------------------//
	SceneObject* ManualObjectFactory::createInstance(const String& name)
	{
		return TITAN_NEW ManualObject(this, name);
	}
	//-------------------------------------------------------------------------------//
	void ManualObjectFactory::destroyInstance(SceneObject* object)
	{
		TITAN_DELETE object;
	}

}