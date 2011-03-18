#include "TitanStableHeader.h"
#include "ManualObject.h"
#include "HardwareBufferManager.h"
#include "TitanShaderEffect.h"

namespace Titan
{

#define TEMP_INITIAL_SIZE 50
#define TEMP_VERTEXSIZE_GUESS sizeof(float) * 12
#define TEMP_INITIAL_VERTEX_SIZE TEMP_VERTEXSIZE_GUESS * TEMP_INITIAL_SIZE
#define TEMP_INITIAL_INDEX_SIZE sizeof(uint32) * TEMP_INITIAL_SIZE

	String ManualObject::mType = "ManualObject";
	//-------------------------------------------------------------//
	ManualObject::ManualObject(const String& name)
		: mName(name), mFirstVertex(true),
		mTempVertexPending(false), mCurrentSection(0),
		mTempVertexBuffer(0),mTempVertexSize(TEMP_INITIAL_VERTEX_SIZE),
		mTempIndexBuffer(0), mTempIndexSize(TEMP_INITIAL_INDEX_SIZE),
		mDeclSize(0)
	{
	}
	//-------------------------------------------------------------//
	ManualObject::~ManualObject()
	{
		clear();
	}
	//-------------------------------------------------------------//
	void ManualObject::_updateRenderableList(SceneMgr::RenderableList& renderableList)
	{
		ManualObjectSectionVector::iterator it = mSectionVector.begin(),
			itend = mSectionVector.end();
		while(it != itend)
		{
			renderableList.push_back(*it);
			++it;
		}
	}
	//-------------------------------------------------------------//
	void ManualObject::begin(RenderData::OperationType ot)
	{
		if(mCurrentSection)
		{
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR,
				"you forgot to call end() to finish last Manual operation",
				"ManualObject::begin");
		}
		mCurrentSection = TITAN_NEW ManualObjectSection(this, ot);
		mSectionVector.push_back(mCurrentSection);

		mFirstVertex = true;
		mDeclSize = 0;
	}
	//-------------------------------------------------------------//
	ManualObject::ManualObjectSection* ManualObject::end()
	{
		if(!mCurrentSection)
		{ 
			TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR,
				"You must call begin() before end()",
				"ManualObject::end()");
		}
		ManualObjectSection* pSection = mCurrentSection;
		RenderData* rd = mCurrentSection->getRenderData();
		if (mTempVertexPending)
		{
			// bake current vertex
			copyTempVertexToBuffer();
		}

		if(rd->vertexData->vertexCount == 0)
		{
			mSectionVector.pop_back();
			TITAN_DELETE mCurrentSection;
		}
		else
		{
			VertexBufferSharedPtr vbuf;

			vbuf = HardwareBufferManager::getSingleton().createVertexBuffer(
				mDeclSize, rd->vertexData->vertexCount,
				HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);

			rd->vertexData->vertexBufferBinding->setBinding(0, vbuf);

			if(rd->useIndex)
			{
				rd->indexData->indexBuffer = HardwareBufferManager::getSingleton().createIndexBuffer(
					rd->indexData->indexCount,
					HardwareBuffer::HBU_STATIC_WRITE_ONLY, true);
			}

			vbuf->writeData(
				0, rd->vertexData->vertexCount * mDeclSize,
				mTempVertexBuffer, true);

			if(rd->useIndex)
			{
				uint16* pIdx = static_cast<uint16*>(rd->indexData->indexBuffer->lock(HardwareBuffer::HBL_DISCARD));
				uint32* pSrc = mTempIndexBuffer;
				for (size_t i = 0; i < rd->indexData->indexCount; i++)
				{
					*pIdx++ = static_cast<uint16>(*pSrc++);
				}
				rd->indexData->indexBuffer->unlock();
			}
		}

		mCurrentSection = 0;
		return pSection;
	}
	//-------------------------------------------------------------//
	void ManualObject::clear()
	{
		resetTempAreas();
		ManualObjectSectionVector::iterator it = mSectionVector.begin(), itend = mSectionVector.end();

		while(it != itend)
		{
			TITAN_DELETE *it;
			++it;
		}

	}
	//-------------------------------------------------------------//
	void ManualObject::resetTempAreas(void)
	{
		TITAN_FREE(mTempVertexBuffer, MEMCATEGORY_GENERAL);
		TITAN_FREE(mTempIndexBuffer, MEMCATEGORY_GENERAL);
		mTempVertexBuffer = 0;
		mTempIndexBuffer = 0;
		mTempVertexSize = TEMP_INITIAL_VERTEX_SIZE;
		mTempIndexSize = TEMP_INITIAL_INDEX_SIZE;
	}
	//-------------------------------------------------------------//
	void ManualObject::position(float x, float y, float z)
	{
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

		mTempVertexPending = true;
	}
	//-------------------------------------------------------------//
	void ManualObject::position(const Vector3& pos)
	{
		position(pos.x, pos.y, pos.z);
	}
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
	void ManualObject::triangle(uint32 i1, uint32 i2, uint32 i3)
	{
		RenderData* rd = mCurrentSection->getRenderData();
		if (rd->operationType !=
			RenderData::OT_TRIANGLE_LIST)
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"This method is only valid on triangle lists",
				"ManualObject::index");
		}

		index(i1);
		index(i2);
		index(i3);
	}
	//-------------------------------------------------------------//
	void ManualObject::quad(uint32 i1, uint32 i2, uint32 i3, uint32 i4)
	{
		// first tri
		triangle(i1, i2, i3);
		// second tri
		triangle(i3, i4, i1);
	}
	//-------------------------------------------------------------//
	void ManualObject::setShaderEffect(ShaderEffectPtr effect)
	{
		ManualObjectSectionVector::iterator it = mSectionVector.begin(), itend = mSectionVector.end();
		while (it != itend)
		{
			//to do
			(*it)->setShaderEffect(effect);
			++it;
		}
	}
	//-------------------------------------------------------------//
	void ManualObject::copyTempVertexToBuffer()
	{
		RenderData* rd = mCurrentSection->getRenderData();
		mTempVertexPending = false;

		if(rd->vertexData->vertexCount == 0)
		{

		}
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

#if 0
			RenderSystem* rs;
			unsigned short dims;
#endif
			switch(elem.getSemantic())
			{
			case VES_POSITION:
				*pFloat++ = mTempVertex.position.x;
				*pFloat++ = mTempVertex.position.y;
				*pFloat++ = mTempVertex.position.z;
				break;
#if 0
			case VES_NORMAL:
				*pFloat++ = mTempVertex.normal.x;
				*pFloat++ = mTempVertex.normal.y;
				*pFloat++ = mTempVertex.normal.z;
				break;
			case VES_TANGENT:
				*pFloat++ = mTempVertex.tangent.x;
				*pFloat++ = mTempVertex.tangent.y;
				*pFloat++ = mTempVertex.tangent.z;
				break;
			case VES_TEXTURE_COORDINATES:
				dims = VertexElement::getTypeCount(elem.getType());
				for (ushort t = 0; t < dims; ++t)
					*pFloat++ = mTempVertex.texCoord[elem.getIndex()][t];
				break;
			case VES_DIFFUSE:
				rs = Root::getSingleton().getRenderSystem();
				if (rs)
					rs->convertColourValue(mTempVertex.colour, pRGBA++);
				else
					*pRGBA++ = mTempVertex.colour.getAsRGBA(); // pick one!
				break;
#endif
			default:
				// nop ?
				break;
			};

		}
	}
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
	void ManualObject::resizeTempIndexBufferIfNeeded(size_t numInds)
	{
		size_t newSize = numInds * sizeof(uint32);
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
			numInds = newSize / sizeof(uint32);
			uint32* tmp = mTempIndexBuffer;
			mTempIndexBuffer = TITAN_ALLOC_T(uint32, numInds, MEMCATEGORY_GENERAL);
			if (tmp)
			{
				memcpy(mTempIndexBuffer, tmp, mTempIndexSize);
				TITAN_FREE(tmp, MEMCATEGORY_GENERAL);
			}
			mTempIndexSize = newSize;
		}
	}

	ManualObject::ManualObjectSection::ManualObjectSection(ManualObject* parent, RenderData::OperationType ot)
		:mParent(parent)
	{
		mRenderData.operationType = ot;
		mRenderData.useIndex = false;
		mRenderData.vertexData = TITAN_NEW VertexData();
	}
	//-------------------------------------------------------------//
	ManualObject::ManualObjectSection::~ManualObjectSection()
	{
		TITAN_DELETE mRenderData.vertexData;
		TITAN_DELETE mRenderData.indexData;
	}	
	//-------------------------------------------------------------//
	void ManualObject::ManualObjectSection::getRenderData(RenderData& rd)
	{
		rd = mRenderData;
	}
	//-------------------------------------------------------------//
	void ManualObject::ManualObjectSection::getTransformMat(Matrix4* transMat)
	{
		*transMat = mParent->_getAttachedNodeFullTransform();
	}

	

	//
	//ManualObject Factory
	//
	String ManualObjectFactory::mType = "ManualObject";
	ManualObjectFactory::ManualObjectFactory()
	{

	}
	//-------------------------------------------------------------//
	ManualObjectFactory::~ManualObjectFactory()
	{

	}
	//-------------------------------------------------------------//
	SceneObject* ManualObjectFactory::createInstance(const String& name)
	{
		return TITAN_NEW ManualObject(name);
	}
	//-------------------------------------------------------------//
	void ManualObjectFactory::destroyInstance(SceneObject* object)
	{
		TITAN_DELETE object;
	}

}