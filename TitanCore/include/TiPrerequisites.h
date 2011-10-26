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

#include "TiDataType.h"

#   pragma warning (disable : 4251)

////yeah, this is learn from unreal~0~
//#include "TiDebugFuncs.h"
//#define check(expr)				{ if(!(expr)) appFailAssert( #expr, __FILE__, __LINE__ ); }

namespace Titan
{
	//Forward declaration
	class Angle;
	class AABB;
	class AutoNamer;
	class AutoParamsSetter;
	class BaseTerrain;
	class BaseTerrainSection;
	class Camera;
	class Color;
	class LogMgr;
	class ChunkTerrain;
	class ChunkTerrainSection;
	class CommonFileSystem;
	class CommonFileSystemFactory;
	class Degree;
	class DynLib;
	class Entity;
	class EntityPrimitive;
	class Exception;
	class Font;
	class FontMgr;
	class Frustum;
	class FileSystem;
	class FileSystemFactory;
	class FileSystemManager;
	class HardwareBuffer;
	class HardwareBufferMgr;
	class Image;
	class IndexData;
	class IndexBuffer;
	class Math;
	class Matrix3;
	class Matrix4;
	class Material;
	class MaterialMgr;
	class ManualObject;
	class ManualObjectFactory;
	class Mesh;
	class OverlayElement;
	class OverlayElementFactory;
	class OverlayTextElement;
	class OverlayTextElementFactory;
	class OverlayPanelElement;
	class OverlayPanelElementFactory;
	class OverlayMgr;
	class Pass;
	class Plane;
	class Plugin;
	class PlaneBoundedVolume;
	class Quadtree;
	class QuadtreeNode;
	class QuadtreeSceneMgr;
	class Quaternion;
	class Root;
	class Radian;
	class Ray;
	class Rect2D;
	class RenderQueue;
	class RenderQueueGroup;
	class Renderer;
	class RenderTarget;
	class RenderWindow;
	class RenderData;
	class Renderable;
	class Resource;
	class ResourceMgr;
	class ResourceGroupMgr;
	class Sphere;
	class ScriptLoader;
	class ScriptCompilerMgr;
	class ScriptTranslator;
	class ScriptTranslatorMgr;
	class SceneMgr;
	class SceneMgrFactory;
	class SceneMgrSelector;
	class SceneObject;
	class SceneObjectFactory;
	class SceneNode;
	class Shader;
	class ShaderUnit;
	class ShaderParams;
	class SubMesh;
	class Texture;
	class TextureMgr;
	class TextureUnit;
	class Timer;
	class Viewport;
	class Vector2;
	class Vector3;
	class Vector4;
	class VertexBuffer;
	class VertexDeclaration;
	class VertexBufferBinding;
	class VertexData;
}

/* Include all the standard header *after* all the configuration
settings have been made.
*/
#include "TiStlHeader.h"
#include "TiMemoryAllocatorConfig.h"

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
	template <typename K, typename P = stdext::hash_compare<K, std::less<K> >, typename A = STLAllocator<K, GeneralAllocPolicy> > 
	struct hash_set 
	{ 
#if TITAN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
		typedef typename stdext::hash_set<K, P, A> type; 
#else
		typedef typename stdext::hash_set<K, P> type; 
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