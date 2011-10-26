#include "TitanStableHeader.h"
#include "TiRenderer.h"
#include "TiRenderTarget.h"
#include "TiLogMgr.h"
#include "TiRenderData.h"
#include "TiVertexBuffer.h"

namespace Titan
{
	Renderer::Renderer()
		: mHeight(0), mWidth(0),mWaitForVSync(false)
	{
		loadDefaultConfigOptions();
	}
	//-------------------------------------------------------------------------------//
	Renderer::~Renderer()
	{
		RenderTargetMap::iterator it = mTargetMap.begin(), itend = mTargetMap.end();
		while (it != itend)
		{
			TITAN_DELETE (it->second);
			++it;
		}
	}
	//-------------------------------------------------------------------------------//
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
			TITAN_EXCEPT_INVALIDPARAMS( str.str());
		}
	}
	//-------------------------------------------------------------------------------//
	RenderTarget* Renderer::getTarget(const String& name) 
	{
		RenderTargetMap::iterator i = mTargetMap.find(name);
		if(i != mTargetMap.end())
		{
			return i->second;
		}
		else
		{
			LogMgr::getSingleton().stream()
				<<"Render Target" << name << "does not be found.";
			return 0;
		}
	}
	//-------------------------------------------------------------------------------//
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
			LogMgr::getSingleton().stream()
				<<"Render Target" << name << "does not be found.";
		}
	}
	//-------------------------------------------------------------------------------//
	void Renderer::addTargetToRender(RenderTarget* target)
	{
		PriorityTargetMap::iterator i = mPriorityTargetMap.begin(), iend = mPriorityTargetMap.end();
		for(;i != iend; ++i)
		{
			if(i->second == target)
			{
				TITAN_EXCEPT_INTERNALERROR( 
					"This RenderTarget has already been added!");
				return;
			}
		}
		mPriorityTargetMap.insert(PriorityTargetMap::value_type(target->getPriority(), target) );

	}
	//-------------------------------------------------------------------------------//
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
	
	//-------------------------------------------------------------------------------//
	void Renderer::updateAllTargets()
	{
		PriorityTargetMap::iterator i, iend = mPriorityTargetMap.end();
		for(i = mPriorityTargetMap.begin(); i != iend; ++i)
		{
			i->second->update();
		}

	}
	//-------------------------------------------------------------------------------//
	void Renderer::swapAllTargetBuffers()
	{
		PriorityTargetMap::iterator i, iend = mPriorityTargetMap.end();
		for(i = mPriorityTargetMap.begin(); i != iend; ++i)
		{
			i->second->swapBuffer();
		}
	}
	//-------------------------------------------------------------------------------//
	void Renderer::_render(const RenderData& rd)
	{
		size_t val;

		if(rd.useIndex)
			val = rd.indexData->indexCount;
		else
			val = rd.vertexData->vertexCount;

		switch(rd.operationType)
		{
		case OT_TRIANGLE_LIST:
			mFaceCount += val / 3;
			break;
		case OT_TRIANGLE_STRIP:
		case OT_TRIANGLE_FAN:
			mFaceCount += val - 2;
			break;
		case OT_POINT_LIST:
		case OT_LINE_LIST:
		case OT_LINE_STRIP:
			break;
		}

		mVertexCount += rd.vertexData->vertexCount;

		
	}
	//-------------------------------------------------------------------------------//
	void Renderer::_beginGeometryCount(void)
	{
		mBatchCount = mFaceCount = mVertexCount = 0;

	}
	//-------------------------------------------------------------------------------//--
	unsigned int Renderer::_getFaceCount(void) const
	{
		return static_cast< unsigned int >( mFaceCount );
	}
	//-------------------------------------------------------------------------------//--
	unsigned int Renderer::_getBatchCount(void) const
	{
		return static_cast< unsigned int >( mBatchCount );
	}
	//-------------------------------------------------------------------------------//--
	unsigned int Renderer::_getVertexCount(void) const
	{
		return static_cast< unsigned int >( mVertexCount );
	}
	//-------------------------------------------------------------------------------//
	void Renderer::_convertProjMatrix(const Matrix4& projSrc, Matrix4& projDst, bool forGpuProgram)
	{
		projDst = projSrc;
	}
	//-------------------------------------------------------------------------------//
	void Renderer::convertColor(const Color& col, uint32* pDest)
	{
		*pDest = VertexElement::convertColor(col, getColorVertexElementType());
	}
	//----------------------------------------------------------------------------//
	void Renderer::_setTextureUnit(size_t stage, TextureUnit* tu)
	{
		_setTexture(stage, tu->getTexture());

		_setTexAddressMode(stage, tu->getTexAddressModeSets());

		_setTexCoordSet(stage, tu->getTexCoordSet());
		
		_setSamplerFilter(stage, tu->getMinFilter(), tu->getMagFilter(), tu->getMipFilter());

		_setTextureMatrix(stage, tu->getTextureTransform());

	}
	//----------------------------------------------------------------------------//
	void Renderer::_setSamplerFilter(uint sampler, FilterOptions minFilter, FilterOptions magFilter, FilterOptions mipFilter)
	{
		_setSamplerFilter(sampler, FT_MIN, minFilter);
		_setSamplerFilter(sampler, FT_MAG, magFilter);
		_setSamplerFilter(sampler, FT_MIP, mipFilter);
	}
	//----------------------------------------------------------------------------//
	void Renderer::_setShader(Shader* shader)
	{

		switch(shader->getShaderType())
		{
		case ST_VERTEX_SHADER:
			{
				mSetVertexShader = true;
				break;
			}
		case ST_PIXEL_SHADER:
			{
				mSetPixelShader = true;
				break;
			}
		}
	}
	//------------------------------------------------------------------------------//
	void Renderer::_clearShader(ShaderType st)
	{
		switch(st)
		{
		case ST_VERTEX_SHADER:
			mSetVertexShader = false;
			break;
		case ST_PIXEL_SHADER:
			mSetPixelShader = false;
			break;
		default: break;
		}
	}
	//------------------------------------------------------------------------------//
	inline bool Renderer::isShaderSet(ShaderType st) const 
	{
		switch(st)
		{
		case ST_VERTEX_SHADER:
			return mSetVertexShader;
		case ST_PIXEL_SHADER:
			return mSetPixelShader;
		}
		return false;
	}
	
	//change set default option into father class, not pure virutual
	void Renderer::loadDefaultConfigOptions()
	{
		ConfigOption resolutionMode;
		resolutionMode.name = "Resolution Mode";
		resolutionMode.value = "800 x 600";

		ConfigOption fullScreen;
		fullScreen.name = "Full Screen";
		fullScreen.value = "No";

		ConfigOption windowTitle;
		windowTitle.name = "Window Title";
		windowTitle.value = "Titan";

		mConfigOptions[resolutionMode.name] = resolutionMode;
		mConfigOptions[fullScreen.name] = fullScreen;
		mConfigOptions[windowTitle.name] = windowTitle;
	}
	// add a base initialize function, every sub class must call this, is this strcuture a little confused?
	RenderWindow* Renderer::initialize()
	{
		ConfigOptionMap::iterator opt = mConfigOptions.find("Full Screen");
		bool fullScreen = opt->second.value == "Yes";

		uint width, height;
		opt = mConfigOptions.find("Resolution Mode");
		String::size_type widthEnd = opt->second.value.find(' ');
		// we know that the height starts 3 characters after the width and goes until the next space
		String::size_type heightEnd = opt->second.value.find(' ', widthEnd+3);
		// Now we can parse out the values
		width = StringConverter::parseInt(opt->second.value.substr(0, widthEnd));
		height = StringConverter::parseInt(opt->second.value.substr(widthEnd+3, heightEnd));

		String windowTitle;
		opt = mConfigOptions.find("Window Title");
		windowTitle = opt->second.value;

		mWidth = width;
		mHeight = height;
		return createRenderWindow(windowTitle, width, height, fullScreen);
	}

}