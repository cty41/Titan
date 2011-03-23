//copied from ogre~
#ifndef __TITANCOMMON__HH
#define __TITANCOMMON__HH

#include "TitanPrerequisites.h"
#include "TitanAny.h"
#include "TitanSharedPtr.h"

namespace Titan
{
	typedef struct  _ConfigOption
	{
		String		name;
		String		value;		
	}ConfigOption;

	/** Structure used to define a box in a 3-D integer space.
	Note that the left, top, and front edges are included but the right, 
	bottom and back ones are not.
	*/
	struct Box : public GeneralAlloc
	{
		size_t left, top, right, bottom, front, back;
		/// Parameterless constructor for setting the members manually
		Box()
			: left(0), top(0), right(1), bottom(1), front(0), back(1)
		{
		}
		/** Define a box from left, top, right and bottom coordinates
		This box will have depth one (front=0 and back=1).
		@param	l	x value of left edge
		@param	t	y value of top edge
		@param	r	x value of right edge
		@param	b	y value of bottom edge
		@note Note that the left, top, and front edges are included 
		but the right, bottom and back ones are not.
		*/
		Box( size_t l, size_t t, size_t r, size_t b ):
		left(l),
			top(t),   
			right(r),
			bottom(b),
			front(0),
			back(1)
		{
			assert(right >= left && bottom >= top && back >= front);
		}
		/** Define a box from left, top, front, right, bottom and back
		coordinates.
		@param	l	x value of left edge
		@param	t	y value of top edge
		@param  ff  z value of front edge
		@param	r	x value of right edge
		@param	b	y value of bottom edge
		@param  bb  z value of back edge
		@note Note that the left, top, and front edges are included 
		but the right, bottom and back ones are not.
		*/
		Box( size_t l, size_t t, size_t ff, size_t r, size_t b, size_t bb ):
		left(l),
			top(t),   
			right(r),
			bottom(b),
			front(ff),
			back(bb)
		{
			assert(right >= left && bottom >= top && back >= front);
		}

		/// Return true if the other box is a part of this one
		bool contains(const Box &def) const
		{
			return (def.left >= left && def.top >= top && def.front >= front &&
				def.right <= right && def.bottom <= bottom && def.back <= back);
		}

		/// Get the width of this box
		size_t getWidth() const { return right-left; }
		/// Get the height of this box
		size_t getHeight() const { return bottom-top; }
		/// Get the depth of this box
		size_t getDepth() const { return back-front; }
	};

	class _DllExport AutoNamer: public GeneralAlloc
	{
	public:
		AutoNamer(const String& prefix)
			:mPrefix(prefix), mCount(0)
		{
		}
		//-------------------------------------------------------------//
		AutoNamer(const AutoNamer& rhs)
			:mPrefix(rhs.mPrefix), mCount(rhs.mCount)
		{
		}
		//-------------------------------------------------------------//
		~AutoNamer(){}
		//-------------------------------------------------------------//
		String		getAutoName()
		{
			std::ostringstream s;
			s << mPrefix << mCount++;
			return s.str();
		}
		//-------------------------------------------------------------//
		void		setPrefix(const String& prefix)
		{
			mPrefix = prefix;
		}

	protected:
		String	mPrefix;
		uint	mCount;
	};

	/** Light shading modes. */
	enum ShadeOptions
	{
		SO_FLAT,
		SO_GOURAUD,
		SO_PHONG
	};

	/** Fog modes. */
	enum FogMode
	{
		/// No fog. Duh.
		FOG_NONE,
		/// Fog density increases  exponentially from the camera (fog = 1/e^(distance * density))
		FOG_EXP,
		/// Fog density increases at the square of FOG_EXP, i.e. even quicker (fog = 1/e^(distance * density)^2)
		FOG_EXP2,
		/// Fog density increases linearly between the start and end distances
		FOG_LINEAR
	};

	/** Hardware culling modes based on vertex winding.
	This setting applies to how the hardware API culls triangles it is sent. */
	enum CullingMode
	{
		/// Hardware never culls triangles and renders everything it receives.
		CULL_NONE = 1,
		/// Hardware culls triangles whose vertices are listed clockwise in the view (default).
		CULL_CLOCKWISE = 2,
		/// Hardware culls triangles whose vertices are listed anticlockwise in the view.
		CULL_ANTICLOCKWISE = 3
	};

	/** Manual culling modes based on vertex normals.
	This setting applies to how the software culls triangles before sending them to the 
	hardware API. This culling mode is used by scene managers which choose to implement it -
	normally those which deal with large amounts of fixed world geometry which is often 
	planar (software culling movable variable geometry is expensive). */
	enum ManualCullingMode
	{
		/// No culling so everything is sent to the hardware.
		MANUAL_CULL_NONE = 1,
		/// Cull triangles whose normal is pointing away from the camera (default).
		MANUAL_CULL_BACK = 2,
		/// Cull triangles whose normal is pointing towards the camera.
		MANUAL_CULL_FRONT = 3
	};

	/** The polygon mode to use when rasterising. */
	enum PolygonMode
	{
		/// Only points are rendered.
		PM_POINTS = 1,
		/// Wireframe models are rendered.
		PM_WIREFRAME = 2,
		/// Solid polygons are rendered.
		PM_SOLID = 3
	};

	/** Defines the frame buffer types. */
	enum FrameBufferType {
		FBT_COLOR  = 0x1,
		FBT_DEPTH   = 0x2,
		FBT_STENCIL = 0x4
	};

	enum VertexElementSemantic 
	{
		/// Position, 3 reals per vertex
		VES_POSITION = 1,
		/// Blending weights
		VES_BLEND_WEIGHTS = 2,
		/// Blending indices
		VES_BLEND_INDICES = 3,
		/// Normal, 3 reals per vertex
		VES_NORMAL = 4,
		/// Diffuse colours
		VES_DIFFUSE = 5,
		/// Specular colours
		VES_SPECULAR = 6,
		/// Texture coordinates
		VES_TEXTURE_COORDINATES = 7,
		/// Binormal (Y axis if normal is Z)
		VES_BINORMAL = 8,
		/// Tangent (X axis if normal is Z)
		VES_TANGENT = 9
	};

	/// Vertex element type, used to identify the base types of the vertex contents
	enum VertexElementType
	{
		VET_FLOAT1 = 0,
		VET_FLOAT2 = 1,
		VET_FLOAT3 = 2,
		VET_FLOAT4 = 3,
		/// alias to more specific color type - use the current rendersystem's color packing
		VET_COLOR = 4,
		VET_SHORT1 = 5,
		VET_SHORT2 = 6,
		VET_SHORT3 = 7,
		VET_SHORT4 = 8,
		VET_UBYTE4 = 9,
		/// D3D style compact colour
		VET_COLOR_ARGB = 10,
		/// GL style compact colour
		VET_COLOR_ABGR = 11
	};


	typedef map<String, ConfigOption>::type ConfigOptionMap;

	typedef map<String, Any>::type AnyMap;		//woo, it's a nice name:)

	typedef	map<String, RenderTarget*>::type	RenderTargetMap;

	typedef vector<String>::type StringVector;

	class _DllExport StringVectorPtr : public SharedPtr<StringVector>
	{
	public:
		StringVectorPtr(): SharedPtr<StringVector>()
		{};

		explicit StringVectorPtr(StringVector* vec, SharedPtrFreeMethod freeMethod = SPFM_DELETE)
			:SharedPtr<StringVector>(vec, freeMethod)
		{}
	};



}

#endif