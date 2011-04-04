#include "TitanStableHeader.h"
#include "TitanOverlayMgr.h"
#include "TitanOverlayElement.h"
#include "TitanRenderQueue.h"

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
	{

	}
	//-------------------------------------------------------------//
	OverlayMgr::~OverlayMgr()
	{
		removeAllFactories();
		removeAllElements();
	}
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
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
	//-------------------------------------------------------------//
	void OverlayMgr::updateRenderQueue(RenderQueue* queue)
	{
		OverlayElementMap::iterator it = mOverlayElementMap.begin(), itend = mOverlayElementMap.end();
		while (it != itend)
		{
			queue->addRenderable(it->second, RenderQueue::RGT_HUD, it->second->getZOrder());
			++it;
		}
	}
}