#include "MayaFuncs.h"

namespace Titan
{
	/*
	maya triangulation helpers triangulatedMesh, edgesPool classes
	by Sebastian Woldanski
	*/
	edgesPool::edgesPool (int numVerts)
	{
		for (int i =0; i< numVerts; i++)
		{
			MIntArray * ar =new MIntArray;
			_vertsPairs.push_back( ar);

			ar =new MIntArray;
			_vertEdges.push_back( ar);
		}
	}


	// ads edge from verts
	bool edgesPool::add (int v1, int v2, int face)
	{

		if (!_vertsPairs[v1] || !_vertsPairs[v2])
			return  false;

		edgesPool::edge * e= find(v1,  v2) ;
		//// look for opposite vert , if found edge already added
		//MIntArray * ar = _vertsPairs[v1];
		//int found =0;
		//for (int i =0; i< ar->length(); i++)
		//{
		//	if ((*ar)[i] == v2)
		//	{
		//		found =1;
		//		break;
		//	}
		//	
		//}

		//if (!found)
		//{
		//	ar = _vertsPairs[v2];
		//	for (int i =0; i< ar->length(); i++)
		//	{
		//		if ((*ar)[i] == v2)
		//		{
		//			found =1;
		//			break;
		//		}
		//	}

		//}
		if (e) 
		{
			// check if face on edge exists
			if (e->f1 == -1 && e->f2 != face) 
				e->f1 = face;
			else if (e->f2 == -1 && e->f1 != face)
				e->f2 = face;
			else
				return false;	

			return false;	
		}
		else
		{
			MIntArray * ar = _vertsPairs[v1];
			ar->append(v2);

			ar = _vertsPairs[v2];
			ar->append(v1);



			edge * e = new edge;
			e->v1 = v1;
			e->v2 = v2;
			if (e->f1 == -1   ) 
				e->f1  =face;
			else 
				e->f2  = face;
			_edges.push_back(e);

			int newEdgeind = ( int )_edges.size() -1;
			ar = _vertEdges[v1];
			ar->append(newEdgeind);

			ar = _vertEdges[v2];
			ar->append(newEdgeind);

			return true;	

		}

	}
	bool edgesPool::findFaces (int v1, int v2, int fases[2] )
	{
		fases[0] = -1;
		fases[1] = -1;


		if (_vertsPairs[v1])
		{

			MIntArray & ar = *_vertsPairs[v1];
			for (int i =0; i< ar.length(); i++)
			{
				if (ar[i] == v2)
				{

					edge * e = _edges[ar[i]];
					if (e )
					{
						fases[0] = e->f1;
						fases[1] = e->f2;

					}
					return true;

				}

			}
		}
		return false;
	}
	edgesPool::edge * edgesPool::find (int v1, int v2  )
	{
		if (_vertsPairs.size() > v1  )
		{
			int ind =-1;
			MIntArray & ar = *_vertsPairs[v1];
			for (int i = 0 ; i < ar.length(); i++)
			{
				if (ar[i] == v2)
				{
					ind = i;
					break;

				}
			}
			if (ind > -1)
			{
				MIntArray & ar = *_vertEdges[v1];
				int eind = ar[ind];
				edge * e = _edges[(*_vertEdges[v1])[ind]];
				return e;

			}
		}
		return NULL;
	}
	void edgesPool::setEdgeSmoothing (MDagPath& mesh  )
	{
		// Add entries, for each edge, to the lookup table
		MItMeshEdge eIt( mesh );
		for ( ; !eIt.isDone(); eIt.next() ) // Iterate over edges.
		{
			bool smooth = eIt.isSmooth();  // Smoothness retrieval for edge.		
			edge * e= find(eIt.index(0), eIt.index(1));
			if (e)
				e->smooth = smooth;
		}
	}
	bool triangulatedMesh::createEdgesPool()
	{
		if (edges)
			delete edges;

		edges =new edgesPool(numVerts);
		if (edges)
		{
			for (uint i =0 ; i < triangles.size(); i++)
			{
				Triangle & t = *triangles[i];
				edges->add(t.Indices[0].pointIndex,t.Indices[1].pointIndex , i);
				edges->add(t.Indices[1].pointIndex,t.Indices[2].pointIndex , i);
				edges->add(t.Indices[2].pointIndex,t.Indices[0].pointIndex , i);

			}
			edges->setEdgeSmoothing(dag);
			return true;
		}
		return false;
	}
	bool triangulatedMesh::triangulate(MDagPath & dagPath)
	{
		MStatus stat;
		//dag.set(_d);
		//mesh = new	MFnMesh(_d);
		dag =  dagPath;
		MFnMesh fnMesh(dag);

		numVerts = 	fnMesh.numVertices();

		MStringArray uvSetNames;
		stat = fnMesh.getUVSetNames(uvSetNames);
		NumUVSets = uvSetNames.length();
		for( int UVIndex = 0; UVIndex < NumUVSets; ++UVIndex )
		{
			fnMesh.getUVs (uArray[UVIndex], vArray[UVIndex] , &uvSetNames[UVIndex] );
		}

		MObjectArray shaders;
		MIntArray shaderIndicesPerPoly;
		stat = fnMesh.getConnectedShaders(0, shaders, shaderIndicesPerPoly);
		if (!stat)
		{		
			return false;
		}
		bool hasColors = false;
		MString colorSetName;
		fnMesh.getCurrentColorSetName ( colorSetName);
		if (fnMesh.getCurrentColorSetName ( colorSetName) && colorSetName.length() )
			hasColors = true;

		MItMeshPolygon polyIter ( dagPath);	

		int c = 0;

		MPointArray _pt_temp;
		MIntArray  tris_vertexListGlobal;
		MIntArray  tris_vertexListLocal;
		MIntArray   verticesIndices;
		// stworzmy liste - face reletive tris verts indexes
		int tangentOffset=0;
		int u=0;

		bool bHasUVSet[NUM_EXTRA_UV_SETS+1];
		for ( ; ! polyIter.isDone(); polyIter.next() )
		{
			if (! polyIter.hasValidTriangulation())
			{			
				//bad geometry
				return false;
			}

			for( int UVIndex = 0; UVIndex < NumUVSets; ++UVIndex )
			{
				if (! polyIter.hasUVs(uvSetNames[UVIndex]))
				{
					bHasUVSet[UVIndex] = false;
				}
				else
				{
					bHasUVSet[UVIndex] = true;
				}
			}


			u =polyIter.index();

			// process triangles
			int triNum;
			polyIter.numTriangles(triNum);

			polyIter.getTriangles( _pt_temp, tris_vertexListGlobal );
			polyIter.getVertices ( verticesIndices);
			tris_vertexListLocal.clear();
			tris_vertexListLocal.setLength(tris_vertexListGlobal.length());

			for ( int i =0 ; i <  tris_vertexListGlobal.length(); i++)
			{	

				for ( int j =0 ; j <  verticesIndices.length(); j++)
				{	
					//	cerr << "		" << tris_vertexListGlobal[i] << " ? " << verticesIndices[j]<<endl;			
					if (tris_vertexListGlobal[i] == verticesIndices[j] )
					{
						tris_vertexListLocal[i] = j;
						break;
					}
				}
			}
			//cerr << "	dupa 2"<<endl;	

			int c=0;
			for ( int i =0 ; i <  triNum; i++)
			{
				triangulatedMesh::Triangle * pt = new triangulatedMesh::Triangle;
				triangulatedMesh::Triangle & t = * pt;
				//cerr << "		gowno 1"<<endl;


				for ( int j =0 ; j <  3; j++)
				{
					t.Indices[j].pointIndex = tris_vertexListGlobal[c];
					//t.Indices[j].normalIndex =polyIter.normalIndex(tris_vertexListLocal[c]);
					int indexuv;

					for( int UVIndex = 0; UVIndex < NumUVSets; ++UVIndex )
					{
						if ( bHasUVSet[ UVIndex ] )
						{
							stat = polyIter.getUVIndex(tris_vertexListLocal[c], indexuv, &uvSetNames[UVIndex]);
							if (stat)
							{
								t.Indices[j].tcIndex[UVIndex] = indexuv;

								if( UVIndex > 0 )
								{
									t.Indices[j].hasExtraUVs[ UVIndex - 1 ] = true;
								}
							}
						}
						else
						{
							t.Indices[j].tcIndex[UVIndex] =0;
						}
					}

					int colorIndex = -1;
					if (hasColors && fnMesh.getFaceVertexColorIndex( u,tris_vertexListLocal[c], colorIndex, &colorSetName) == MS::kSuccess) 
					{
						t.Indices[j].vertexColor = colorIndex;
					}	



					c++;

				}
				//cerr << "		gowno 2"<<endl;

				if (shaderIndicesPerPoly[u] == -1)
				{

					t.ShadingGroupIndex =-1;
				}
				else
					t.ShadingGroupIndex =shaderIndicesPerPoly[u];

				triangles.push_back(pt);

			}


		}

		return true;
	}

	void MeshProcessor::createNewSmoothingGroupsFromTriangulatedMesh( triangulatedMesh& mesh )
	{
		// Create our edge lookup table and initialize all entries to NULL.

		int edgeTableSize = mesh.numVerts;
		int numPolygons = ( int )mesh.triangles.size();
		if( edgeTableSize < 1)
			return; //SafeGuard.

		mVertEdgePools.clear();
		mVertEdgePools.pop_back( VertEdges() );  

		// Add entries, for each edge, to the lookup table
		for (int i =0; i <  mesh.edges->_edges.size(); i++ ) // Iterate over edges.
		{

			edgesPool::edge * e =  mesh.edges->_edges[i];
			bool smooth =e->smooth;  // Smoothness retrieval for edge.
			addEdgeInfo( e->v1, e->v2, smooth );  // Adds info for edge running from vertex elt.index(0) to elt.index(1) into mVertEdgePools.
			MayaEdge* elem = findEdgeInfo( e->v1, e->v2 );
			if ( elem )  // Null if no edges found for vertices a and b. 
			{

				elem->polyIds[0] = e->f1; // Add poly index to poly table for this edge.
				elem->polyIds[1] = e->f2; // If first slot already filled, fill the second face.  Assumes each edge only has 2 faces max.

			}
		}
		// Fill mFacePools table: for each face, a pool of smoothly touching faces and a pool of nonsmoothly touching ones.	

		mFacePools.AddZeroed( numPolygons );
		for (int p =0; p <  mesh.triangles.size(); p++ ) // Iterate over edges.
		{

			triangulatedMesh::Triangle * t =  mesh.triangles[p];
			for ( int vid=0; vid<3;vid++ ) 
			{
				int a = t->Indices[vid].pointIndex;
				int b = t->Indices[ vid==(2) ? 0 : vid+1 ].pointIndex;

				MayaEdge* Edge = findEdgeInfo( a, b );
				if ( Edge )
				{
					int FaceA = Edge->polyIds[0];
					int FaceB = Edge->polyIds[1];
					int TouchingFaceIdx = -1;

					if( FaceA == p )  
						TouchingFaceIdx = FaceB;
					else 
						if( FaceB == p ) 
							TouchingFaceIdx = FaceA;

					if( TouchingFaceIdx >= 0)
					{
						if( Edge->smooth )
						{
							mFacePools[p].SmoothFaces.AddUniqueItem(TouchingFaceIdx);							
						}
						else
						{
							mFacePools[p].HardFaces.AddUniqueItem(TouchingFaceIdx);					
						}
					}
				}
			}
		}



		mPolyProcessFlags.clear();
		mPolyProcessFlags.AddZeroed( numPolygons );
		{for ( int i=0; i< numPolygons; i++ ) 
		{ 
			mPolyProcessFlags[i] = NO_SMOOTHING_GROUP; 
		}}    

		//
		// Convert all from edges into smoothing groups.  Essentially flood fill it. but als check each poly if it's been processed yet.
		//
		mFaceSmoothingGroups.AddZeroed( numPolygons );
		int SmoothParts = 0;
		mCurrentGroup = 0;
		{for ( int pid=0; pid<numPolygons; pid++ ) 
		{
			if( mPolyProcessFlags[pid] == NO_SMOOTHING_GROUP )
			{			
				fillSmoothFaces( pid );
				SmoothParts++;
			}
		}}

		//
		// Fill mGroupTouchPools: for each group, this notes which other groups touch it or overlap it in any way.
		//
		mGroupTouchPools.AddZeroed( mCurrentGroup ); 
		for( int f=0; f< numPolygons; f++)
		{
			TArray<int> TempTouchPool;

			// This face's own groups..
			{for( int i=0; i< mFaceSmoothingGroups[f].Groups.Num(); i++)
			{
				TempTouchPool.AddUniqueItem( mFaceSmoothingGroups[f].Groups[i] );
			}}

			// And those of the neighbours, whether touching or not..
			{for( int n=0; n< mFacePools[f].HardFaces.Num(); n++)
			{
				int FaceIdx = mFacePools[f].HardFaces[n];
				for( int i=0; i< mFaceSmoothingGroups[ FaceIdx].Groups.Num(); i++)
				{
					TempTouchPool.AddUniqueItem( mFaceSmoothingGroups[FaceIdx].Groups[i] );
				}
			}}
			{for( int n=0; n< mFacePools[f].SmoothFaces.Num(); n++)
			{
				int FaceIdx = mFacePools[f].SmoothFaces[n];
				for( int i=0; i< mFaceSmoothingGroups[ FaceIdx].Groups.Num(); i++)
				{
					TempTouchPool.AddUniqueItem( mFaceSmoothingGroups[FaceIdx].Groups[i] );
				}
			}}

			// Then for each element add all other elements uniquely to its GroupTouchPool (includes its own..)
			for( int g=0; g< TempTouchPool.Num(); g++)
			{
				for( int t=0; t< TempTouchPool.Num(); t++ )
				{
					mGroupTouchPools[ TempTouchPool[g] ].Groups.AddUniqueItem( TempTouchPool[t] );
				}
			}
		}


		// Distribute final smoothing groups by carefully checking against already assigned groups in
		// each group's touchpools. Note faces can have many multiple touching groups so the
		// 4-colour theorem doesn't apply, but it should at least help us limit the smoothing groups to 32.

		int numRawSmoothingGroups = mGroupTouchPools.Num();
		int HardClashes = 0;
		int MaxGroup = 0;
		for( int gInit =0; gInit <mGroupTouchPools.Num(); gInit++)
		{
			mGroupTouchPools[gInit].FinalGroup = -1;
		}

		for( int g=0; g<mGroupTouchPools.Num(); g++ )
		{		
			int FinalGroupCycle = 0;
			for( int SGTries=0; SGTries<32; SGTries++ )
			{
				int Clashes = 0;
				// Check all touch groups. If none has been assigned this FinalGroupCycle number, we're done otherwise, try another one.
				for( int t=0; t< mGroupTouchPools[g].Groups.Num(); t++ )
				{
					Clashes += ( mGroupTouchPools[ mGroupTouchPools[g].Groups[t] ].FinalGroup == FinalGroupCycle ) ? 1: 0;
				}
				if( Clashes == 0 )
				{
					// Done for this smoothing group.
					mGroupTouchPools[g].FinalGroup = FinalGroupCycle;
					break; 
				}			
				FinalGroupCycle = (FinalGroupCycle+1)%32;
			}
			if( mGroupTouchPools[g].FinalGroup == -1)
			{
				HardClashes++;			
				mGroupTouchPools[g].FinalGroup = 0; // Go to default group.
			}		
			MaxGroup = max( MaxGroup, mGroupTouchPools[g].FinalGroup );
			if( (g!=0) && (( g & 65535 )==0) )
			{
				MayaLog(" Busy merging smoothing groups... %i ",g);
			}
		}

		if( HardClashes > 0) 
		{
			MayaLog(" Warning: [%i] smoothing group reassignment errors.",HardClashes );
		}


		// Resulting face smoothing groups remapping to final groups.
		{for( int p=0; p< numPolygons; p++)
		{
			// This face's own groups..
			for( int i=0; i< mFaceSmoothingGroups[p].Groups.Num(); i++)
			{			
				//mFaceSmoothingGroups[p].Groups[i] = RemapperArray[ mFaceSmoothingGroups[p].Groups[i]  ];
				mFaceSmoothingGroups[p].Groups[i] = mGroupTouchPools[ mFaceSmoothingGroups[p].Groups[i] ].FinalGroup;
			}
		}}

		// Finally, let's copy them into mPolySmoothingGroups.
		mPolySmoothingGroups.clear();
		mPolySmoothingGroups.AddExactZeroed( mFaceSmoothingGroups.Num() );
		{for( int i=0; i< mPolySmoothingGroups.Num(); i++)
		{
			// OR-in all the smoothing bits.
			for( int s=0; s< mFaceSmoothingGroups[i].Groups.Num(); s++)
			{ 
				mPolySmoothingGroups[i]  |=  ( 1 << (  mFaceSmoothingGroups[i].Groups[s]  ) );
			}
		}}

		//MayaLog(" Faces %i  - Smoothing groups before reduction:  %i   after reduction: %i  ",numPolygons,  numRawSmoothingGroups, MaxGroup );

	}
	// PCF END


	//
	// New smoothing group extraction. Rewritten to eliminate hard-to-track-down anomalies inherent in the Maya SDK's mesh processor smoothing group extraction code.
	//
	void MeshProcessor::createNewSmoothingGroups( MDagPath& mesh )
	{
		// Create our edge lookup table and initialize all entries to NULL.
		MFnMesh fnMesh( mesh );
		int edgeTableSize = fnMesh.numVertices(); 
		int numPolygons = fnMesh.numPolygons();
		if( edgeTableSize < 1)
			return; //SafeGuard.

		mVertEdgePools.clear();
		mVertEdgePools.AddZeroed( edgeTableSize );  

		// Add entries, for each edge, to the lookup table
		MItMeshEdge eIt( mesh );
		for ( ; !eIt.isDone(); eIt.next() ) // Iterate over edges.
		{
			bool smooth = eIt.isSmooth();  // Smoothness retrieval for edge.
			addEdgeInfo( eIt.index(0), eIt.index(1), smooth );  // Adds info for edge running from vertex elt.index(0) to elt.index(1) into mVertEdgePools.
		}

		MItMeshPolygon pIt( mesh );
		for ( ; !pIt.isDone(); pIt.next() )
		{
			int pvc = pIt.polygonVertexCount();
			for ( int v=0; v<pvc; v++ )
			{
				// Circle around polygons assigning the edge IDs  to edgeinfo's 			
				int a = pIt.vertexIndex( v );
				int b = pIt.vertexIndex( v==(pvc-1) ? 0 : v+1 ); // wrap

				MayaEdge* elem = findEdgeInfo( a, b );
				if ( elem )  // Null if no edges found for vertices a and b. 
				{
					int edgeId = pIt.index();

					if ( INVALID_ID == elem->polyIds[0] ) 
					{
						elem->polyIds[0] = edgeId; // Add poly index to poly table for this edge.
					}
					else 
					{
						elem->polyIds[1] = edgeId; // If first slot already filled, fill the second face.  Assumes each edge only has 2 faces max.
					}                                    
				}
			}
		}

		// Fill mFacePools table: for each face, a pool of smoothly touching faces and a pool of nonsmoothly touching ones.	
		mFacePools.AddZeroed( numPolygons );
		{for ( int p=0; p< numPolygons; p++ ) 
		{		
			MIntArray vertexList; 
			fnMesh.getPolygonVertices( p, vertexList );
			int vcount = vertexList.length();		

			// Walk around this polygon. accumulate all smooth and sharp bounding faces..
			for ( int vid=0; vid<vcount;vid++ ) 
			{
				int a = vertexList[ vid ];
				int b = vertexList[ vid==(vcount-1) ? 0 : vid+1 ];
				MayaEdge* Edge = findEdgeInfo( a, b );
				if ( Edge )
				{
					int FaceA = Edge->polyIds[0];
					int FaceB = Edge->polyIds[1];
					int TouchingFaceIdx = -1;

					if( FaceA == p )  
						TouchingFaceIdx = FaceB;
					else 
						if( FaceB == p ) 
							TouchingFaceIdx = FaceA;

					if( TouchingFaceIdx >= 0)
					{
						if( Edge->smooth )
						{
							mFacePools[p].SmoothFaces.AddUniqueItem(TouchingFaceIdx);							
						}
						else
						{
							mFacePools[p].HardFaces.AddUniqueItem(TouchingFaceIdx);					
						}
					}								
				}
			}
		}}

		mPolyProcessFlags.clear();
		mPolyProcessFlags.AddZeroed( numPolygons );
		{for ( int i=0; i< numPolygons; i++ ) 
		{ 
			mPolyProcessFlags[i] = NO_SMOOTHING_GROUP; 
		}}    

		//
		// Convert all from edges into smoothing groups.  Essentially flood fill it. but als check each poly if it's been processed yet.
		//
		mFaceSmoothingGroups.AddZeroed( numPolygons );
		int SmoothParts = 0;
		mCurrentGroup = 0;
		{for ( int pid=0; pid<numPolygons; pid++ ) 
		{
			if( mPolyProcessFlags[pid] == NO_SMOOTHING_GROUP )
			{			
				fillSmoothFaces( pid );
				SmoothParts++;
			}
		}}

		//
		// Fill mGroupTouchPools: for each group, this notes which other groups touch it or overlap it in any way.
		//
		mGroupTouchPools.AddZeroed( mCurrentGroup ); 
		for( int f=0; f< numPolygons; f++)
		{
			TArray<int> TempTouchPool;

			// This face's own groups..
			{for( int i=0; i< mFaceSmoothingGroups[f].Groups.Num(); i++)
			{
				TempTouchPool.AddUniqueItem( mFaceSmoothingGroups[f].Groups[i] );
			}}

			// And those of the neighbours, whether touching or not..
			{for( int n=0; n< mFacePools[f].HardFaces.Num(); n++)
			{
				int FaceIdx = mFacePools[f].HardFaces[n];
				for( int i=0; i< mFaceSmoothingGroups[ FaceIdx].Groups.Num(); i++)
				{
					TempTouchPool.AddUniqueItem( mFaceSmoothingGroups[FaceIdx].Groups[i] );
				}
			}}
			{for( int n=0; n< mFacePools[f].SmoothFaces.Num(); n++)
			{
				int FaceIdx = mFacePools[f].SmoothFaces[n];
				for( int i=0; i< mFaceSmoothingGroups[ FaceIdx].Groups.Num(); i++)
				{
					TempTouchPool.AddUniqueItem( mFaceSmoothingGroups[FaceIdx].Groups[i] );
				}
			}}

			// Then for each element add all other elements uniquely to its GroupTouchPool (includes its own..)
			for( int g=0; g< TempTouchPool.Num(); g++)
			{
				for( int t=0; t< TempTouchPool.Num(); t++ )
				{
					mGroupTouchPools[ TempTouchPool[g] ].Groups.AddUniqueItem( TempTouchPool[t] );
				}
			}
		}


		// Distribute final smoothing groups by carefully checking against already assigned groups in
		// each group's touchpools. Note faces can have many multiple touching groups so the
		// 4-colour theorem doesn't apply, but it should at least help us limit the smoothing groups to 32.

		int numRawSmoothingGroups = mGroupTouchPools.Num();
		int HardClashes = 0;
		int MaxGroup = 0;
		for( int gInit =0; gInit <mGroupTouchPools.Num(); gInit++)
		{
			mGroupTouchPools[gInit].FinalGroup = -1;
		}

		for( int g=0; g<mGroupTouchPools.Num(); g++ )
		{		
			int FinalGroupCycle = 0;
			for( int SGTries=0; SGTries<32; SGTries++ )
			{
				int Clashes = 0;
				// Check all touch groups. If none has been assigned this FinalGroupCycle number, we're done otherwise, try another one.
				for( int t=0; t< mGroupTouchPools[g].Groups.Num(); t++ )
				{
					Clashes += ( mGroupTouchPools[ mGroupTouchPools[g].Groups[t] ].FinalGroup == FinalGroupCycle ) ? 1: 0;
				}
				if( Clashes == 0 )
				{
					// Done for this smoothing group.
					mGroupTouchPools[g].FinalGroup = FinalGroupCycle;
					break; 
				}			
				FinalGroupCycle = (FinalGroupCycle+1)%32;
			}
			if( mGroupTouchPools[g].FinalGroup == -1)
			{
				HardClashes++;			
				mGroupTouchPools[g].FinalGroup = 0; // Go to default group.
			}		
			MaxGroup = max( MaxGroup, mGroupTouchPools[g].FinalGroup );
			if( (g!=0) && (( g & 65535 )==0) )
			{
				MayaLog(" Busy merging smoothing groups... %i ",g);
			}
		}

		if( HardClashes > 0) 
		{
			MayaLog(" Warning: [%i] smoothing group reassignment errors.",HardClashes );
		}


		// Resulting face smoothing groups remapping to final groups.
		{for( int p=0; p< numPolygons; p++)
		{
			// This face's own groups..
			for( int i=0; i< mFaceSmoothingGroups[p].Groups.Num(); i++)
			{			
				//mFaceSmoothingGroups[p].Groups[i] = RemapperArray[ mFaceSmoothingGroups[p].Groups[i]  ];
				mFaceSmoothingGroups[p].Groups[i] = mGroupTouchPools[ mFaceSmoothingGroups[p].Groups[i] ].FinalGroup;
			}
		}}

		// Finally, let's copy them into mPolySmoothingGroups.
		mPolySmoothingGroups.clear();
		mPolySmoothingGroups.AddExactZeroed( mFaceSmoothingGroups.Num() );
		{for( int i=0; i< mPolySmoothingGroups.Num(); i++)
		{
			// OR-in all the smoothing bits.
			for( int s=0; s< mFaceSmoothingGroups[i].Groups.Num(); s++)
			{ 
				mPolySmoothingGroups[i]  |=  ( 1 << (  mFaceSmoothingGroups[i].Groups[s]  ) );
			}
		}}

		//MayaLog(" Faces %i  - Smoothing groups before reduction:  %i   after reduction: %i  ",numPolygons,  numRawSmoothingGroups, MaxGroup );

	}



	//
	// Flood-fills mesh with placeholder smoothing group numbers.
	//  - Start from a face and reach all faces smoothly connected with "mCurrentGroup"
	//
	//  
	//   ==> General rule: new triangle: get set of all groups bounding it across a sharp boundary; 
	//             if mCurrentGroup is not in the sharpboundgroups,  just assign mCurrentGroup; store surrounding unprocessed faces on stack; pick top of stack to process. 
	//             if mCurrentGroup is in there -> see if our pre-filled Groups  have none in common with SharpBoundGroups;  
	//             if none in common, we can just use the Groups
	//             if any in common, we assign a NEW ++LatestGroup, discard Groups, and put the LatestGroup into all our surrounding tris' Groups.
	//
	//      - This should propagate  edge groups, and keep everything smooth that's smooth.
	//
	//

	// Count common elements between two TArrays of INTs. Assumes the elements are unique (otherwise the result may be too big ).
	int CountCommonElements( TArray<int>& ArrayOne, TArray<int>& ArrayTwo )
	{
		int Counter = 0;
		for(int i=0;i<ArrayOne.Num(); i++)
		{
			if( ArrayTwo.Contains( ArrayOne[i] ) )
				Counter++;
		}
		return Counter;
	}

	// See if there are any common elements at all.
	int AnyCommonElements( TArray<int>& ArrayOne, TArray<int>& ArrayTwo )
	{
		int LastTested = -9999999;
		for(int i=0;i<ArrayOne.Num(); i++)
		{
			if( ArrayOne[i] != LastTested ) // Avoid unnecessary testing of same item.
			{
				if( ArrayTwo.Contains( ArrayOne[i] ) )
				{
					return 1;
				}
				LastTested = ArrayOne[i];
			}		
		}
		return 0;
	}


	// PCF BEGIN - not using MFnMesh& fnMesh
	void MeshProcessor::fillSmoothFaces( int polyid  )
		//PCF END
	{		
		TArray<int> TodoFaceStack;
		TodoFaceStack.AddItem( polyid ); // Guaranteed to have been a NO_SMOOTHING_GROUP marked face.

		int LatestGroup = mCurrentGroup;

		while( TodoFaceStack.Num() )
		{
			// Get top of stack.
			int StackTopIdx = TodoFaceStack.Num() -1;
			int ThisFaceIdx = TodoFaceStack[ StackTopIdx ];		
			TodoFaceStack.DelIndex( StackTopIdx );

			mPolyProcessFlags[ThisFaceIdx] = 1; // Mark as processed.

			// mCurrentGroup, our first choice, is not in any of the groups assigned to any of the faces of the 'sharp' connecting face set ? Then we'll try to use it.
			int SharpSetCurrMatches = 0;
			{for( int t=0; t< mFacePools[ ThisFaceIdx].HardFaces.Num(); t++ )
			{
				int HardFaceAcross = mFacePools[ ThisFaceIdx].HardFaces[t] ;
				SharpSetCurrMatches += mFaceSmoothingGroups[ HardFaceAcross  ].Groups.Contains( mCurrentGroup );				
			}}

			// Do our "pre-filled" groups match any sharply connected groups ? 
			int SharpSetGroupMatches = 0;
			{for( int t=0; t< mFacePools[ ThisFaceIdx].HardFaces.Num(); t++ )
			{
				int HardFaceAcross = mFacePools[ ThisFaceIdx].HardFaces[t] ;
				//SharpSetGroupMatches += CountCommonElements(  mFaceSmoothingGroups[ThisFaceIdx].Groups, mFaceSmoothingGroups[HardFaceAcross].Groups );
				SharpSetGroupMatches += AnyCommonElements(  mFaceSmoothingGroups[ThisFaceIdx].Groups, mFaceSmoothingGroups[HardFaceAcross].Groups );
			}}

			UBOOL NewGroup = true;
			int SplashGroup = mCurrentGroup;		

			// No conflicts, then we can assign the default 'currentgroup' and move on. 
			if( SharpSetCurrMatches == 0  && SharpSetGroupMatches == 0 )
			{			
				mFaceSmoothingGroups[ ThisFaceIdx ].Groups.AddItem( mCurrentGroup );
				NewGroup = false;			
			}
			else
			{				
				// If mCurrentGroup _does_ match across sharp, but none of our pre-setgroups match AND there are more than 0 - we have a definite candidate already.			
				if( ( SharpSetCurrMatches > 0 )  && (SharpSetGroupMatches == 0) &&  (mFaceSmoothingGroups[ThisFaceIdx].Groups.Num() > 0 ) ) 
				{
					NewGroup=false;
					// Plucked  our existing, first pre-set group as a candidate to splash over our environment as we progress - always ensures smoothess with our smooth neighbors.
					SplashGroup = mFaceSmoothingGroups[ThisFaceIdx].Groups[0];
				}
			}

			if( NewGroup )
			{
				LatestGroup++;
				SplashGroup = LatestGroup;
				mFaceSmoothingGroups[ThisFaceIdx].Groups.clear();  // We don't need any pre-set groups for smoothness, since we'll saturate our environment.
				mFaceSmoothingGroups[ThisFaceIdx].Groups.AddItem( SplashGroup ); // Set group in this face.
			}

			// If we added ANY single new group that's not our default group, we splash it all over our _non-processed_ environment, because we need to 
			// smoothly mesh with our default environment,  always.  This way, 'odd' groups progress themselves as necessary. 
			if( SplashGroup != mCurrentGroup )
			{
				for( int t=0; t< mFacePools[ ThisFaceIdx ].SmoothFaces.Num(); t++ )
				{
					int SmoothFaceAcross = mFacePools[ ThisFaceIdx ].SmoothFaces[t];			
					// Only do it in case of == LatestGroup - ensures no corruption with anything else around.. - or when come from pre-loaded Groups; only forward to un-processed faces.
					if(  (SplashGroup == LatestGroup) || ( mPolyProcessFlags[ SmoothFaceAcross ] < 0 ) ) 
					{
						mFaceSmoothingGroups[ SmoothFaceAcross ].Groups.AddUniqueItem( SplashGroup );
					}
				}
			}

			// Now add all unprocessed, unqueued,  smoothly touching ones to our todo stack.
			for( int t=0; t< mFacePools[ ThisFaceIdx ].SmoothFaces.Num(); t++ )
			{
				int SmoothFaceAcross = mFacePools[ ThisFaceIdx ].SmoothFaces[t];
				if( mPolyProcessFlags[ SmoothFaceAcross ]  == NO_SMOOTHING_GROUP ) // ONLY unprocessed ones are added, and setting QUEUED  will ensure they're uniquely added.
				{
					TodoFaceStack.AddItem( SmoothFaceAcross );
					mPolyProcessFlags[ SmoothFaceAcross ]  = QUEUED_FOR_SMOOTHING; 
				}
			}
		} // While still faces to process..

		// Currentgroup up to Latestgroup encompass all new unique groups assigned in this run for this floodfilled area.

		// When done, make mCurrentGroup higher than any we've used.
		mCurrentGroup = ++LatestGroup;

	}



	//
	// Adds a new edge info element to the vertex table.
	//
	void MeshProcessor::addEdgeInfo( int v1, int v2, bool smooth )
	{
		MayaEdge NewEdge;
		NewEdge.vertId = v2;
		NewEdge.smooth = smooth;
		NewEdge.polyIds[0] = INVALID_ID;
		NewEdge.polyIds[1] = INVALID_ID;
		// Add it always.
		mVertEdgePools[v1].Edges.AddItem( NewEdge );
	}

	//
	// Find an edge connecting vertex 1 and vertex 2.
	//
	MayaEdge* MeshProcessor::findEdgeInfo( int v1, int v2 )
	{
		// Look in mVertEdgePools[v1] for v2..
		//   or in mVertEdgePools[v2] for v1..
		int EdgeIndex;

		EdgeIndex = 0;
		while( EdgeIndex < mVertEdgePools[v1].Edges.Num() ) 
		{
			if( mVertEdgePools[v1].Edges[EdgeIndex].vertId == v2 )
			{
				return &(mVertEdgePools[v1].Edges[EdgeIndex]);
			}
			EdgeIndex++;
		}

		EdgeIndex = 0;
		while( EdgeIndex < mVertEdgePools[v2].Edges.Num() ) 
		{
			if( mVertEdgePools[v2].Edges[EdgeIndex].vertId == v1 )
			{
				return &(mVertEdgePools[v2].Edges[EdgeIndex]);
			}
			EdgeIndex++;
		}

		return NULL;
	}

	float GetMayaTimeStart()
	{
		MTime start( MAnimControl::minTime().value(), MTime::uiUnit() );
		return start.value();
	}

	float GetMayaTimeEnd()
	{
		MTime end( MAnimControl::maxTime().value(), MTime::uiUnit() );
		return end.value();
	}

	int whereInMobjectArray( MObjectArray& MArray, MObject& LocateObject )
	{
		int KnownIndex = -1;
		for( long t=0; t < MArray.length(); t++)
		{
			if( MArray[t] == LocateObject ) // comparison legal ?
				KnownIndex = t;
		}
		return KnownIndex;
	}

	void SetMayaSceneTime(float CurrentTime)
	{
		MTime NowTime;
		NowTime.setValue(CurrentTime);
		MGlobal::viewFrame( NowTime );
	}


}