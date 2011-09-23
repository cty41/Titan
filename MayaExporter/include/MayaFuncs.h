#ifndef __MAYA_FUNCTIONS__HH
#define __MAYA_FUNCTIONS__HH

#include "MayaHeader.h"
#include "StlHeaders.h"
#include "TitanHelpers.h"

namespace Titan
{
	/*
	edgesPool - builds edges structure for smoothing groups of triangulated polys
	*/
	class edgesPool
	{
	public:
		edgesPool (int numVerts);

		~edgesPool()
		{
			destroy();
		};



		void destroy()
		{
			for (uint i =0; i< _edges.size(); i++)		
				if (_edges[i]) delete _edges[i];
			for (uint i =0; i< _vertsPairs.size(); i++)		
				if (_vertsPairs[i]) delete _vertsPairs[i];
			for (uint i =0; i< _vertEdges.size(); i++)		
				if (_vertEdges[i]) delete _vertEdges[i];
		}
		typedef struct edge
		{
			int	v1;
			int v2;
			int f1;
			int f2;
			bool smooth;
			edge()
			{
				v1=-1;
				v2=-1;
				f1=-1;
				f2=-1;
				smooth =true;
			};

		};


		bool add (int v1, int v2, int faceIndex);
		void setEdgeSmoothing (MDagPath & );	// sets smoothing flag via maya polygon edge iterator
		bool findFaces (int v1, int v2, int fases[2]  ); // returns edge faces from verts
		edge * find (int v1, int v2  ); // returns valid edge from verts


		std::vector<edge *>	_edges;
		std::vector<MIntArray *>	_vertsPairs; // size of numVerts
		std::vector<MIntArray *>	_vertEdges; // size of numVerts

	private:



	};
	// The number of additional supported UV sets besides the manditory 1
#define NUM_EXTRA_UV_SETS 3
	/*
	triangulatedMesh 
	triangulates maya dag path and builds tructure of indexes

	*/
	class triangulatedMesh
	{
	public:
		triangulatedMesh():edges(nullptr)
		{
			numVerts =0;
		};
		~triangulatedMesh()
		{
			destroy();
		};

		void destroy()
		{
			if (edges)
				delete edges;
			for (uint i =0; i< triangles.size(); i++)
			{
				if(triangles[i]) delete triangles[i];

			}
		}

		struct Index
		{
			int	pointIndex;
			int tcIndex[NUM_EXTRA_UV_SETS+1];
			bool hasExtraUVs[NUM_EXTRA_UV_SETS];
			int vertexColor;
			Index()
			{

				memset( hasExtraUVs, 0, sizeof( bool ) * NUM_EXTRA_UV_SETS );
				vertexColor =-1;

			}


		};
		typedef struct Triangle
		{
			Index	Indices[3];			
			int		ShadingGroupIndex;	// index for ShadingGroups
		};

		bool triangulate(MDagPath & _d);
		bool createEdgesPool();

		std::vector <Triangle *>	triangles;

		MFloatArray uArray[NUM_EXTRA_UV_SETS+1]; 
		MFloatArray vArray[NUM_EXTRA_UV_SETS+1];

		int numVerts;

		int NumUVSets;

		edgesPool * edges;

		bool exportNormals;




	private:
		MDagPath  dag;

	};

	// Edge info to go in a TArray for each indiv. vertex.
	struct MayaEdge
	{
		int     polyIds[2];  // ID's of polygons that reference this edge.
		int		vertId; // Second vertex
		bool	smooth; // is this edge smooth ?
	};

	// For a vertex, a TArray with all edges.
	struct VertEdges
	{
		VertEdges()
		{

		}
		std::vector<MayaEdge> Edges;
	};

	struct GroupList
	{
		std::vector<int> Groups;	
		int FinalGroup;
	};

	// Face environment for each face - pools of all touching smooth- and hard-bounded face indices.
	class FaceEnviron
	{
	public:
		std::vector<int> SmoothFaces;
		std::vector<int> HardFaces;
	};

	// Helper class to convert edge smoothing properties to face smoothing groups.
	class MeshProcessor
	{
	public:

		// Edge lookup methods
		void            buildEdgeTable( MDagPath& mesh );
		void            createNewSmoothingGroups( MDagPath& mesh );	
		//PCF BEGIN
		void            createNewSmoothingGroupsFromTriangulatedMesh( triangulatedMesh& );	
		//PCF END
		void            addEdgeInfo( int v1, int v2, bool smooth );
		MayaEdge*       findEdgeInfo( int v1, int v2);
		bool			smoothingAlgorithm( int, MFnMesh& fnMesh );	
		// PCF BEGIN - not using MFnMesh& fnMesh
		void            fillSmoothFaces( int polyid );
		//PCF END

		// Structors
		MeshProcessor()
		{
			mCurrentGroup = 1;
		}

		~MeshProcessor()
		{
			// Deallocate the multi-level-TArrays properly.
			for ( uint v=0; v<mVertEdgePools.size(); v++ )
			{
				mVertEdgePools[v].Edges.clear();
			}
			mVertEdgePools.clear();	

			for( uint g=0; g<mGroupTouchPools.size(); g++)
			{
				mGroupTouchPools[g].Groups.clear();
			}
			mGroupTouchPools.clear();

			for( uint f=0; f<mFaceSmoothingGroups.size(); f++)
			{
				mFaceSmoothingGroups[f].Groups.clear();
			}
			mFaceSmoothingGroups.clear();

			for( uint h=0; h<mFacePools.size(); h++)
			{
				mFacePools[h].SmoothFaces.clear();
				mFacePools[h].HardFaces.clear();
			}
			mFacePools.clear();

		}

		void RemapGroupTouchTools( int source, int dest )
		{
			// Remap all elements;
			for( uint i=0; i< mGroupTouchPools.size(); i++)
			{
				int Changes = 0;
				for( uint j=0; j< mGroupTouchPools[i].Groups.size(); j++)
				{
					if ( mGroupTouchPools[i].Groups[j] == source )
					{
						mGroupTouchPools[i].Groups[j] = dest;
						Changes++;
					}
				}

				// Eliminate possible duplicates by copying it into a unique array.
				if( Changes )
				{
					std::vector<int> NewGroups;
					for( uint j=0; j<mGroupTouchPools[i].Groups.size(); j++)
					{
						bool bNotAdd = false;
						for(auto itr = NewGroups.begin(); itr != NewGroups.end(); ++itr)
						{
							if(mGroupTouchPools[i].Groups[j] == *itr)
							{
								bNotAdd= true;
								break;
							}
						}
						if(!bNotAdd)
							NewGroups.push_back( mGroupTouchPools[i].Groups[j] );
					}
					mGroupTouchPools[i].Groups.clear();
					for( uint n=0; n<NewGroups.size(); n++)
					{
						mGroupTouchPools[i].Groups.push_back( NewGroups[n] );
					}
				}			
			}

			// Merge source Pool elements into dest pool.
			for( uint j=0; j< mGroupTouchPools[source].Groups.size(); j++)
			{
				int item = mGroupTouchPools[source].Groups[j];
				bool bNotAdd = false;
				for(auto itr = mGroupTouchPools[dest].Groups.begin(); itr != mGroupTouchPools[dest].Groups.end(); ++itr)
				{
					if(item == *itr)
					{
						bNotAdd= true;
						break;
					}
				}
				if(!bNotAdd)
					mGroupTouchPools[dest].Groups.push_back( item );
			}

			// Delete 'dest' element lager..
			mGroupTouchPools[source].Groups.clear();		
		}


	public:

		std::vector< long >					mPolySmoothingGroups;
		std::vector< GroupList >			mFaceSmoothingGroups;

	private:

		std::vector< int >					mPolyProcessFlags;
		std::vector<FaceEnviron>			mFacePools;
		std::vector< VertEdges >			mVertEdgePools;	    

		std::vector< GroupList >			mGroupTouchPools;	
		std::vector< int >					mFacesTodo;	 // Bookmarks for flood filling group assignment.
		std::vector< int >					mSmoothEnvironmentFaces; // Smoothly joined neighbor faces for current face.
		std::vector< int >					mHardEnvironmentFaces; // Non-smoothly joined neighbor faces for current face.

		int					mCurrentGroup;

		long				mNextSmoothingGroup;
		long				mCurrSmoothingGroup;
		bool				mNewSmoothingGroup;
	};

	// Misc  Maya specific.
	float GetMayaTimeStart();
	float GetMayaTimeEnd();
	// Get an actual shader, from a higher (shading group) node.
	//MObject findShader( MObject& setNode );  
	// Get a texture name (forward slashes converted to backward) from a shader.
	//int GetTextureNameFromShader( const MObject& shaderNode, char* BitmapFileName, int MaxChars ); 

	int whereInMobjectArray( MObjectArray& MArray, MObject& LocateObject);

	void SetMayaSceneTime(float CurrentTime);
}
#endif