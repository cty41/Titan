#include "TitanStableHeader.h"
#include "TitanOverlayMgr.h"
#include "TitanOverlayElement.h"
#include "TiRenderQueue.h"
#include "TitanOverlayPanelElement.h"
#include "TitanOverlayTextElement.h"
#include "TiViewport.h"

namespace Titan
{
	template<> OverlayMgr* Singleton<OverlayMgr>::ms_Singleton = 0;
	OverlayMgr& OverlayMgr::getSingleton()
	{
		assert( ms_Singleton );  return ( *ms_Singleton );
	}
	OverlayMgr* OverlayMgr::getSingletonPtr()
	{
		return ms_Singleton;
	}

	OverlayMgr::OverlayMgr()
		:mViewportHeight(0), mViewportWidth(0),
		mViewportChanged(false)
	{
		addFactory(TITAN_NEW OverlayPanelElementFactory());
		addFactory(TITAN_NEW OverlayTextElementFactory());
	}
	//-------------------------------------------------------------------------------//
	OverlayMgr::~OverlayMgr()
	{
		removeAllFactories();
		removeAllElements();
	}
	//-------------------------------------------------------------------------------//
	OverlayElement* OverlayMgr::createElement(const String& type, const String& name)
	{
		OverlayElementMap::iterator it = mOverlayElementMap.find(name);
		if(it != mOverlayElementMap.end())
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"the overlay element called this name : "  + name + "has existed!",
				"OverlayMgr::createElement");
			return NULL;
		}
		else
		{
			OverlayElementFactoryMap::iterator fit = mOverlayElementFactoryMap.find(type);
			if(fit == mOverlayElementFactoryMap.end())
			{
				TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
					"overlay element Factory with this type : " + type + "does not exist!",
					"OverlayMgr::createElement");
				return NULL;
			}
			else
			{
				OverlayElement* element = fit->second->createInstance(name);
				mOverlayElementMap[name] = element;
				return element;
			}
		}

	}
	//-------------------------------------------------------------------------------//
	void OverlayMgr::removeElement(const String& name)
	{
		OverlayElementMap::iterator it = mOverlayElementMap.find(name);
		if(it == mOverlayElementMap.end())
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"the overlay element called this name : "  + name + "does not exist!",
				"OverlayMgr::removeElement");
		}
		else
		{
			TITAN_DELETE(it->second);
			mOverlayElementMap.erase(it);
		}
	}
	void OverlayMgr::removeElement(OverlayElement* element)
	{
		OverlayElementMap::iterator it = mOverlayElementMap.find(element->getName());
		if(it == mOverlayElementMap.end())
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"the overlay element called this name : "  + element->getName() + "does not exist!",
				"OverlayMgr::removeElement");
		}
		else
		{
			TITAN_DELETE(it->second);
			mOverlayElementMap.erase(it);
		}
	}
	//-------------------------------------------------------------------------------//
	void OverlayMgr::removeAllElements()
	{
		OverlayElementMap::iterator it = mOverlayElementMap.begin(), itend = mOverlayElementMap.end();
		while (it != itend)
		{
			TITAN_DELETE(it->second);
			++it;
		}
		mOverlayElementMap.clear();
	}
	//-------------------------------------------------------------------------------//
	OverlayElement* OverlayMgr::findElement(const String& name)
	{
		OverlayElementMap::iterator it = mOverlayElementMap.find(name);
		if(it == mOverlayElementMap.end())
		{
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
				"We do not find overlay element named" + name,
				"OverlayMgr::findElement");
			return NULL;
		}
		else
			return it->second;
	}
	//-------------------------------------------------------------------------------//
	void OverlayMgr::renameElement(const String& old, const String& newName)
	{
		OverlayElementMap::iterator it = mOverlayElementMap.find(old);
		if(it == mOverlayElementMap.end())
		{
			TITAN_EXCEPT(Exception::EXCEP_ITEM_NOT_FOUND,
				"We do not find overlay element named" + old,
				"OverlayMgr::findElement");

		}
		else
		{
			OverlayElement* element = it->second;
			mOverlayElementMap.erase(it);
			mOverlayElementMap.insert(OverlayElementMap::value_type(newName, element));
		}
	}
	//-------------------------------------------------------------------------------//
	void OverlayMgr::addFactory(OverlayElementFactory* factory)
	{
		OverlayElementFactoryMap::iterator it = mOverlayElementFactoryMap.find(factory->getTypeName());
		if (it == mOverlayElementFactoryMap.end())
		{
			mOverlayElementFactoryMap[factory->getTypeName()] = factory;
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"the overlay element factory with this type: "  + factory->getTypeName() + "has existed!",
				"OverlayMgr::addFactory");
		}
	}
	//-------------------------------------------------------------------------------//
	void OverlayMgr::removeFactory(const String& type)
	{
		OverlayElementFactoryMap::iterator it = mOverlayElementFactoryMap.find(type);
		if(it != mOverlayElementFactoryMap.end())
		{
			TITAN_DELETE(it->second);
			mOverlayElementFactoryMap.erase(it);
		}
		else
		{
			TITAN_EXCEPT(Exception::EXCEP_INVALID_PARAMS,
				"the overlay element factory with this type: "  + type + "does not exist!",
				"OverlayMgr::removeFactory");
		}
	}
	//-------------------------------------------------------------------------------//
	void OverlayMgr::removeAllFactories()
	{
		OverlayElementFactoryMap::iterator it = mOverlayElementFactoryMap.begin(), itend = mOverlayElementFactoryMap.end();
		while(it != itend)
		{
			TITAN_DELETE(it->second);
			++it;
		}
		mOverlayElementFactoryMap.clear();
	}
	//-------------------------------------------------------------------------------//
	void OverlayMgr::updateRenderQueue(RenderQueue* queue, Viewport* vp)
	{
		if(mViewportHeight != vp->getActualHeight() ||
			mViewportWidth != vp->getActualWidth())
		{
			mViewportChanged = true;
			mViewportHeight = vp->getActualHeight();
			mViewportWidth = vp->getActualWidth();
		}
		else
			mViewportChanged = false;


		OverlayElementMap::iterator it = mOverlayElementMap.begin(), itend = mOverlayElementMap.end();
		while (it != itend)
		{
			it->second->update();
			it->second->updateRenderQueue(queue);
			++it;
		}
	}
}