#ifndef __TITAN_MAYA_SCENETRANSLATOR
#define __TITAN_MAYA_SCENETRANSLATOR

#include "StlHeaders.h"
#include "MayaFuncs.h"
#include "TitanHelpers.h"
#define NOMINMAX
#include <windows.h>


#define NEWMAYAEXPORT


//
// Static mesh triangle.
//
class GFace
{
public:

	int				WedgeIndex[3];	     			// point to three vertices in the vertex list.	
	int				Wedge2Index[3];				// point to three vertices in the (second UV set) vertex list.
	long			SmoothingGroups;			// 32-bit flag for smoothing groups AND Lod-bias calculation.
	long			AuxSmoothingGroups;         //
	std::vector<int>		MaterialIndices;	    // Materials can be anything.

	GFace()
	{
		memset( this, 0, sizeof( GFace));
	}

	~GFace()
	{
		MaterialIndices.clear();
	}

	void operator=( GFace& G ) 
	{	
		WedgeIndex[0] = G.WedgeIndex[0];
		WedgeIndex[1] = G.WedgeIndex[1];
		WedgeIndex[2] = G.WedgeIndex[2];

		Wedge2Index[0] = G.Wedge2Index[0];
		Wedge2Index[1] = G.Wedge2Index[1];
		Wedge2Index[2] = G.Wedge2Index[2];

		SmoothingGroups = G.SmoothingGroups;
		AuxSmoothingGroups = G.AuxSmoothingGroups;		

		MaterialIndices.clear();
		for(uint mIndex = 0; mIndex<G.MaterialIndices.size(); mIndex++)
		{
			MaterialIndices.push_back( G.MaterialIndices[mIndex] );
		}
	}
};
//
// Static mesh vertex with 'unlimited' texturing info.
//
class GWedge
{
public:
	int		PointIndex;	 // Index to a point.
	float	U,V;
	int		MaterialIndex;		
};

//
// Per-vertex color property.
//
class GColor
{
public:
	float R,G,B,A;
	GColor()
	{
		R=0.0f;
		G=0.0f;
		B=0.0f;
		A=1.0f;
	}
	GColor( float ColorR, float ColorG, float ColorB, float ColorA)
	{
		R = ColorR;
		G= ColorG;
		B = ColorB;
		A = ColorA;
	}
};

// Static mesh material.
//
class GMaterial
{		
public:
	std::string	Name;       // Name we'll export it with.
	std::string	BitmapName; //
	float   Opacity;    //
	float   R,G,B;      // possible color

	GMaterial()
	{
		memset( this, 0, sizeof(GMaterial));
	}
	~GMaterial()
	{
		Name.clear();
		BitmapName.clear();
	}
};

class PrimGroupList
{
public:
	std::vector<int> Groups;	

	PrimGroupList()	
	{
		memset( this, 0, sizeof( PrimGroupList));
	}
	~PrimGroupList()
	{
		Groups.clear();		
	}

	//void operator=( PrimGroupList& P ) 
	//{
	//	Groups.clear();
	//	for( uint i=0; i<P.Groups.size(); i++)
	//	{
	//		Groups.push_back( P.Groups[i] );
	//	}
	//}

};

// Static mesh export.
class GeometryPrimitive
{

public:
	void* node;         // Original scene node.
	std::string   Name;       //	
	int  Selected;

	std::vector<Vector3>		Vertices;
	std::vector<GWedge>		Wedges;
	std::vector<GFace>		Faces;
	std::vector<GWedge>		Wedges2;
	std::vector<GColor>		VertColors;		



	std::vector<PrimGroupList> mFaceSmoothingGroups; // Local smoothing groups.

	// ctor
	GeometryPrimitive()
	{
		memset( this, 0, sizeof( GeometryPrimitive ));
	}
	// dtor 
	~GeometryPrimitive()
	{		
		for(uint i=0; i< mFaceSmoothingGroups.size(); i++)
		{
			mFaceSmoothingGroups[i].Groups.clear();
		}
		mFaceSmoothingGroups.clear();

		for(uint f=0; f< Faces.size(); f++)
		{
			Faces[f].MaterialIndices.clear();
		}
		Faces.clear();

		Wedges.clear();		
		Vertices.clear();
		VertColors.clear();
		Wedges2.clear();

		Name.clear();
	}

	double _rotation[3];
	double _translation[3];
};


class NodeInfo
{
public:
	void*	node;
	int		IsBone;   // in the sense of a valid hierarchical node for physique...
	int     IsCulled;
	int		IsBiped;
	int		IsRoot;
	int     IsSmooth; // Any smooth-skinned mesh.
	int     IsSkin;
	int     IsDummy;
	int     IsMaxBone;
	int     LinksToSkin;
	int     HasMesh;
	int     HasTexture;
	int     IsSelected;
	int     IsRootBone;
	int     NoExport; // untie links from NoExport bones..

	int     InSkeleton;
	int     ModifierIdx; // Modifier for this node (Maya skincluster usage)
	int     IgnoreSubTree;

	// These are enough to recover the hierarchy.
	int  	ParentIndex;
	int     NumChildren;

	int     RepresentsBone;

	//ctor
	NodeInfo()
	{
		memset(this,0, sizeof(NodeInfo));
	}
	~NodeInfo()
	{
	}
};

class FrameSeq
{
public:
	int			StartFrame;
	int			EndFrame;
	typedef std::vector<int> FrameVec;
	FrameVec	Frames;
	FrameSeq()
	{
		memset(this, 0, sizeof(FrameSeq));
	}

	void AddFrame(int i)
	{
		if( i>=StartFrame && i<=EndFrame) Frames.push_back(i);
	}

	int GetFrame(int index)
	{
		return Frames[index];
	}

	int GetTotal()
	{
		return Frames.size();
	}

	void clear()
	{
		Frames.empty();
	}
};

class SkinInf
{
public:
	void*	Node;
	int		IsSmoothSkinned;
	int		IsTextured;
	int		SeparateMesh;
	int		SceneIndex;
	int     IsVertexAnimated;
};

class SceneTranslator
{
public:
	SceneTranslator();

	~SceneTranslator();

	bool processMayaScene();

	int serializeSceneTree();

	void getSceneInfo();

	void clearScenePointers();

	void storeNodeTree();

	int findSkinCluster(MObject& obj);

	int processStaticMesh(int treeIdx);

	int	matchNodeToIndex(void* ANode)
	{
		for (uint t=0; t<mSerialTree.size(); t++)
		{
			if ( (void*)(mSerialTree[t].node) == (void*)ANode) return t;
		}
		return -1; // no matching node found.
	}

	int getChildNodeIndex(int PIndex,int ChildNumber)
	{
		int Sibling = 0;
		for (uint t=0; t<mSerialTree.size(); t++)
		{
			if (mSerialTree[t].ParentIndex == PIndex) 
			{
				if( Sibling == ChildNumber ) 
				{	
					return t; // return Nth child.
				}
				Sibling++;
			}
		}
		return -1; // no matching node found.		
	}

	int	getParentNodeIndex(int CIndex)
	{
		return mSerialTree[CIndex].ParentIndex; //-1 if none
	}

	bool isObjectSelected(const MDagPath& path);

	bool isMeshSelected(const MDagPath& path);

public:
	typedef std::vector <SkinInf>  SkinInfVec;
	typedef std::vector< NodeInfo >  NodeInfoVec;

	typedef std::vector<GeometryPrimitive> StaticPrimitiveVec;
	typedef std::vector<GMaterial> StaticMaterialVec;

protected:
	MObjectArray	mAxSceneObjects;
	MObjectArray	mAxSkinClusters;
	MObjectArray	mAxShaders;
	MObjectArray	mAxBlendShapes;

	StaticPrimitiveVec	mStaticPrimitives;
	StaticMaterialVec	mStaticMeshMaterials;

	void*			mOurSkin;

	SkinInfVec		mOurSkins;
	void*			mOurRootBone;
	int				mRootBoneIndex;
	// LH: deprecating this variable - 
	// This is almost identical as mTotalBones except
	// this does not include noexport/ignored parameter
	// but used to calculate wrong size of data for keytrack. 
	// This will be replaced with mTotalBones
	int				mOurBoneTotal;

	// Simple array of nodes for the whole scene tree.
	NodeInfoVec		mSerialTree; 

	// Physique skin data
	int				mPhysiqueNodes;

	// Skeletal links to skin:
	int				mLinkBones;
	int				mTotalSkinLinks;

	// Misc evaluation counters
	int				mGeomMeshes;
	int				mHierarchies;
	int				mTotalBones;
	int				mTotalDummies;
	int				mTotalBipBones;
	int				mTotalMaxBones;
	// More stats:
	int				mTotalVerts;
	int				mTotalFaces;
	int				mDuplicateBones;

	// Animation
	//TimeValue		mTimeStatic; // for evaluating any objects at '(0)' reference.
	//TimeValue		mFrameRange;
	//TimeValue		mFrameStart;
	//TimeValue		mFrameEnd;

	float			mTimeStatic; // for evaluating any objects at '(0)' reference.
	float			mFrameRange;
	float			mFrameStart;
	float			mFrameEnd;

	int				mFrameTicks;
	int				mOurTotalFrames;
	float			mFrameRate; // frames per second !

	FrameSeq		mFrameList; // Total frames == mFrameList.Num()

	bool			mQuickSaveDisk;
	bool			mDoForceRate;

	bool			mDoAppendVertex, mDoScaleVertex, mDoReplaceUnderscores;

	float			mPersistentRate;
	float			mVertexExportScale;

	bool			mDoConvertSmooth, mDoForceTriangles, mDoUnderscoreSpace, mDoGeomAsFilename, mDoUntexturedAsCollision, 
		mDoSuppressPopups, mDoSelectedStatic, mDoConsolidateGeometry;

	bool			mDoExportInObjectSpace;

};


#endif