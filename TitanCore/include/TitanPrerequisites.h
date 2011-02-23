#ifndef __PREREQUISITES_HH
#define __PREREQUISITES_HH

#include "TitanConfig.h"
#include <string>

#if _DEBUG
#	if	TITAN_MEMORY_TRACKER_MODE
#		define TITAN_MEMORY_TRACKER 1
#	else 
#		define TITAN_MEMORY_TRACKER 0
#	endif
#endif

namespace Titan
{
	typedef unsigned int uint32;
	typedef unsigned short uint16;
	typedef unsigned char uint8;
	typedef int int32;
	typedef short int16;
	typedef char int8;

	typedef unsigned char uchar;
	typedef unsigned short ushort;
	typedef unsigned int uint;
	typedef unsigned long ulong;

	class Root;
	class Exception;
	class ConsoleDebugger;
	class FileSystem;
	class FileSystemFactory;
	class CommonFileSystem;
	class CommonFileSystemFactory;
	class Renderer;
	class RenderTarget;
	class RenderWindow;
	class Plugin;
	class DynLib;
	class Renderer;
	class Timer;
	class Angle;
	class AABB;
	class AutoNamer;
	class Camera;
	class Color;
	class Degree;
	class Math;
	class Matrix3;
	class Matrix4;
	class Plane;
	class PlaneBoundedVolume;
	class Quaternion;
	class Radian;
	class Ray;
	class RenderQueue;
	class RenderQueueGroup;
	class Sphere;
	class Viewport;
	class Vector2;
	class Vector3;
	class Vector4;
	class HardwareBuffer;
	class VertexBuffer;
	class VertexDeclaration;
	class IndexBuffer;
	class VertexBufferBinding;
	class HardwareBufferManager;
	class VertexData;
	class IndexData;
	class RenderData;
	class Renderable;
	class Resource;
	class ResourceMgr;
	class ResourceGroupManager;
	class SceneMgr;
	class SceneMgrFactory;
	class SceneMgrSelector;
	class SceneObject;
	class SceneObjectFactory;
	class SceneNode;
	class ManualObject;
	class ManualObjectFactory;
	class Frustum;
}

/* Include all the standard header *after* all the configuration
settings have been made.
*/
#include "StlHeader.h"
#include "MemoryAllocatorConfig.h"

namespace Titan
{
	template <typename T, typename A = STLAllocator<T, GeneralAllocPolicy> > 
	struct deque 
	{ 
#if TITAN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::deque<T, A> type;    
#else
		typedef typename std::deque<T> type;    
#endif
	}; 

	template <typename T, typename A = STLAllocator<T, GeneralAllocPolicy> > 
	struct vector 
	{ 
#if TITAN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::vector<T, A> type;    
#else
		typedef typename std::vector<T> type;    
#endif
	}; 

	template <typename T, typename A = STLAllocator<T, GeneralAllocPolicy> > 
	struct list 
	{ 
#if TITAN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::list<T, A> type;    
#else
		typedef typename std::list<T> type;    
#endif
	}; 

	template <typename T, typename P = std::less<T>, typename A = STLAllocator<T, GeneralAllocPolicy> > 
	struct set 
	{ 
#if TITAN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::set<T, P, A> type;    
#else
		typedef typename std::set<T, P> type;    
#endif
	}; 

	template <typename K, typename V, typename P = std::less<K>, typename A = STLAllocator<std::pair<const K, V>, GeneralAllocPolicy> > 
	struct map 
	{ 
#if TITAN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::map<K, V, P, A> type; 
#else
		typedef typename std::map<K, V, P> type; 
#endif
	}; 

	template <typename K, typename V, typename P = std::less<K>, typename A = STLAllocator<std::pair<const K, V>, GeneralAllocPolicy> > 
	struct multimap 
	{ 
#if TITAN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::multimap<K, V, P, A> type; 
#else
		typedef typename std::multimap<K, V, P> type; 
#endif
	}; 

//hash_map
#if _MSC_VER >1300 && !defined(_STLP_MSVC)
		template <typename K, typename V, typename P = stdext::hash_compare<K, std::less<K> >, typename A = STLAllocator<std::pair<const K, V>, GeneralAllocPolicy> > 
		struct hash_map 
		{ 
	#if TITAN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
			typedef typename stdext::hash_map<K, V, P, A> type; 
	#else
			typedef typename stdext::hash_map<K, V, P> type; 
	#endif
		};
#else
	template <typename K, typename V, typename P = std::less<K>, typename A = STLAllocator<std::pair<const K, V>, GeneralAllocPolicy> > 
	struct hash_map 
	{ 
#if TITAN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::map<K, V, P, A> type; 
#else
		typedef typename std::map<K, V, P> type; 
#endif
	};
#endif

	//hash_map
#if _MSC_VER >1300 && !defined(_STLP_MSVC)
	template <typename K, typename V, typename P = stdext::hash_compare<K, std::less<K> >, typename A = STLAllocator<std::pair<const K, V>, GeneralAllocPolicy> > 
	struct hash_set 
	{ 
#if TITAN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
		typedef typename stdext::hash_set<K, V, P, A> type; 
#else
		typedef typename stdext::hash_set<K, V, P> type; 
#endif
	};
#else
	template <typename K, typename V, typename P = std::less<K>, typename A = STLAllocator<std::pair<const K, V>, GeneralAllocPolicy> > 
	struct hash_set 
	{ 
#if TITAN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
		typedef typename std::set<K, V, P, A> type; 
#else
		typedef typename std::set<K, V, P> type; 
#endif
	};
#endif


#if TITAN_WCHAR
	typedef std::wstring _StringBase;
#else
	typedef std::string _StringBase;
#endif



#if TITAN_WCHAR
	typedef std::basic_stringstream<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t> > _StringStreamBase;
#else
	typedef std::basic_stringstream<char,std::char_traits<char>,std::allocator<char> > _StringStreamBase;
#endif

	typedef _StringBase String;
	typedef _StringStreamBase StringStream;
	typedef StringStream stringstream;


}

#endif