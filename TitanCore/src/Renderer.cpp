#include "TitanStableHeader.h"
#include "Renderer.h"
#include "RenderTarget.h"
#include "ConsoleDebugger.h"
#include "RenderData.h"

namespace Titan
{
	Renderer::Renderer()
		: mHeight(0), mWidth(0),mWaitForVSync(false)
	{
	}
	//-------------------------------------------------------------//
	Renderer::~Renderer()
	{
	}
	//-------------------------------------------------------------//
	void Renderer::setConfigOption(const String& first, const String& second)
	{
		ConfigOptionMap::iterator it = mConfigOptions.find( first );
		if(it != mConfigOptions.end())
		{
			it->second.value = second;
		}
		else
		{
			StringStream str;
			str<<"option : ' "<<first<<"' does not exist.";
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS, str.str(), "void D3D9Renderer::setConfigOption(const String& first, const String& second)");
		}
	}
	//-------------------------------------------------------------//
	RenderTarget* Renderer::getTarget(const String& name) 
	{
		RenderTargetMap::iterator i = mTargetMap.find(name);
		if(i != mTargetMap.end())
		{
			return i->second;
		}
		else
		{
			ConsoleDebugger::getSingleton().stream()
				<<"Render Target" << name << "does not be found.";
			return 0;
		}
	}
	//-------------------------------------------------------------//
	void Renderer::deleteTarget(const String& name)
	{
		RenderTargetMap::iterator i = mTargetMap.find(name);
		if(i != mTargetMap.end())
		{
			PriorityTargetMap::iterator i2 = mPriorityTargetMap.begin(), iend = mPriorityTargetMap.end();
			for(;i2 != iend; ++i2)
			{
				if(i2->second == i->second)
				{
					mPriorityTargetMap.erase(i2);
					break;
				}
			}
			mTargetMap.erase(i);
		}
		else
		{
			ConsoleDebugger::getSingleton().stream()
				<<"Render Target" << name << "does not be found.";
		}
	}
	//-------------------------------------------------------------//
	void Renderer::addTargetToRender(RenderTarget* target)
	{
		PriorityTargetMap::iterator i = mPriorityTargetMap.begin(), iend = mPriorityTargetMap.end();
		for(;i != iend; ++i)
		{
			if(i->second == target)
			{
				TITAN_EXCEPT(Exception::EXCEP_INTERNAL_ERROR, 
					"This RenderTarget has already been added!", "void Renderer::addTargetToRender(RenderTarget* target)");
				return;
			}
		}
		mPriorityTargetMap.insert(PriorityTargetMap::value_type(target->getPriority(), target) );

	}
	//-------------------------------------------------------------//
	void Renderer::delTargetFromRender(RenderTarget* target)
	{
		PriorityTargetMap::iterator i = mPriorityTargetMap.begin(), iend = mPriorityTargetMap.end();
			for(;i != iend;++i)
			{
				if(i->second == target)
				{
					mPriorityTargetMap.erase(i);
					break;
				}	
			}
	}
	
	//-------------------------------------------------------------//
	void Renderer::updateAllTargets()
	{
		PriorityTargetMap::iterator i, iend = mPriorityTargetMap.end();
		for(i = mPriorityTargetMap.begin(); i != iend; ++i)
		{
			i->second->update();
		}

	}
	//-------------------------------------------------------------//
	void Renderer::swapAllTargetBuffers()
	{
		PriorityTargetMap::iterator i, iend = mPriorityTargetMap.end();
		for(i = mPriorityTargetMap.begin(); i != iend; ++i)
		{
			i->second->swapBuffer();
		}
	}
	//-------------------------------------------------------------//
	void Renderer::_render(const RenderData& rd)
	{
		size_t val;

		if(rd.useIndex)
			val = rd.indexData->indexCount;
		else
			val = rd.vertexData->vertexCount;

		switch(rd.operationType)
		{
		case RenderData::OT_TRIANGLE_LIST:
			mFaceCount += val / 3;
			break;
		case RenderData::OT_TRIANGLE_STRIP:
		case RenderData::OT_TRIANGLE_FAN:
			mFaceCount += val - 2;
			break;
		case RenderData::OT_POINT_LIST:
		case RenderData::OT_LINE_LIST:
		case RenderData::OT_LINE_STRIP:
			break;
		}

		mVertexCount += rd.vertexData->vertexCount;

		
	}
	//-------------------------------------------------------------//
	void Renderer::_convertProjMatrix(Matrix4& projSrc, Matrix4& projDst)
	{
		projDst = projSrc;
	}
}