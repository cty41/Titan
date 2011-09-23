#include "SceneTranslator.h"

namespace Titan
{
	SceneTranslator::SceneTranslator()
		:mDoSelectedStatic(true), mDoExportInObjectSpace(true)
	{

	}

	SceneTranslator::~SceneTranslator()
	{

	}

	bool SceneTranslator::processMayaScene()
	{
		MStatus stat;

		serializeSceneTree();
		getSceneInfo();

		//Maya Shaders
		mAxShaders.clear();

		//
		// Go over the nodes list and digest each of them into the mStaticPrimitives GeometryPrimitive array
		//	
		int NumMeshes = 0;
		for(uint i=0; i<mSerialTree.size(); i++)
		{		
			MFnDagNode DagNode = mAxSceneObjects[ (int)mSerialTree[i].node ]; 
			//no use now
			//int IsRoot = ( TempActor.MatchNodeToSkeletonIndex( (void*) i ) == 0 );

			// If mesh, determine 
			int MeshFaces = 0;
			int MeshVerts = 0;
			if( mSerialTree[i].IsSkin )
			{
				MFnMesh MeshFunction( mAxSceneObjects[ (int)mSerialTree[i].node ], &stat );
				MeshVerts = MeshFunction.numVertices();
				MeshFaces = MeshFunction.numPolygons();
			}

			if( MeshFaces )
			{
				NumMeshes++;
				//DLog.Logf("Mesh stats:  Faces %4i  Verts %4i \n\n", MeshFaces, MeshVerts );
				if ( mSerialTree[i].IsSkin )
				{
					// Process this primitive.
					if (processStaticMesh(i)== -1)
						return false;
				}
			}
		} //Serialtree



		return true;
	}

	//return node num
	int SceneTranslator::serializeSceneTree()
	{
		mOurSkin = nullptr;
		mOurRootBone = nullptr;

		mFrameStart = GetMayaTimeStart(); // in ticks..
		mTimeStatic = std::max(mFrameStart, 0.0f);
		mFrameEnd   = GetMayaTimeEnd();   // in ticks..
		mFrameTicks = 1;
		mFrameRate  = 30.0f;// TODO: Maya framerate retrieval. 

		if( mDoForceRate ) mFrameRate = mPersistentRate; //Forced rate.

		mSerialTree.clear();

		int ChildCount = 0;

		// Store all (relevant) nodes into mSerialTree (actually, indices into AXSceneObjects. )
		storeNodeTree();

		// Update basic nodeinfo contents : parent/child relationships.
		// Beware (later) that in Maya there is alwas a transform object between geometry and joints.
		for(uint  i=0; i<mSerialTree.size(); i++)
		{	
			int ParentIndex = -1;
			MFnDagNode DagNode = mAxSceneObjects[(int)mSerialTree[i].node]; // object to DagNode... 

			MObject ParentNode = DagNode.parent(0);
			// Match to mAxSceneObjects
			//int InSceneIdx = mAxSceneObjects.Where(ParentNode);
			int InSceneIdx = whereInMobjectArray( mAxSceneObjects, ParentNode );

			ParentIndex = matchNodeToIndex( (void*) InSceneIdx ); // find parent
			mSerialTree[i].ParentIndex = ParentIndex;


			int NumChildren = 0;
			NumChildren = DagNode.childCount();
			mSerialTree[i].NumChildren = NumChildren;
			//#todo Note: this may be wrong when we ignoderd children in the selection.

			if( NumChildren > 0 ) ChildCount+=NumChildren;

			// Update selected status
			// mSerialTree[i].IsSelected = ((INode*)mSerialTree[i].node)->Selected();
			// MAX:  Establish our hierarchy by way of Serialtree indices... upward only.
			// mSerialTree[i].ParentIndex = MatchNodeToIndex(((INode*)mSerialTree[i].node)->GetParentNode());
			// mSerialTree[i].NumChildren = ( ((INode*)mSerialTree[i].node)->NumberOfChildren() );

			// Print tree node name... note: is a pointer to an inner name, don't change content !
			// char* NodeNamePtr;
			// DtShapeGetName( (int)mSerialTree[i].node,  &NodeNamePtr );
			// MayaLog("Node#: %i  Name: [%s] NodeID: %i ",i,NodeNamePtr, (int)mSerialTree[i].node );

			MDagPath ObjectPath;
			DagNode.getPath( ObjectPath );
			// stat = skinCluster.getPathAtIndex(index,skinPath);
			// Check whether selected:

			if( isObjectSelected ( ObjectPath ))
				mSerialTree[i].IsSelected = 1;

		}

		// PopupBox("Serialtree num: %i children: %i",mSerialTree.Num(), ChildCount );
		return mSerialTree.size();
	}

	void SceneTranslator::getSceneInfo()
	{
		MStatus stat;

		mPhysiqueNodes = 0;
		mTotalBones    = 0;
		mTotalDummies  = 0;
		mTotalBipBones = 0;
		mTotalMaxBones = 0;

		mGeomMeshes = 0;
		int MayaJointCount = 0;

		mLinkBones = 0;
		mTotalSkinLinks = 0;

		if (mSerialTree.size() == 0) return ;

		SetMayaSceneTime( mTimeStatic );	

		mOurSkins.clear();

		for(uint i=0; i<mSerialTree.size(); i++)
		{					
			MFnDagNode DagNode = mAxSceneObjects[ (int)mSerialTree[i].node ]; 
			MObject	Object = DagNode.object();

			// A mesh or geometry ? #TODO check against untextured geometry...
			mSerialTree[i].HasMesh = ( Object.hasFn(MFn::kMesh) ? 1:0 );
			mSerialTree[i].HasTexture = mSerialTree[i].HasMesh;

			// Check for a bone candidate.
			mSerialTree[i].IsBone = ( Object.apiType() == MFn::kJoint ) ? 1:0;

			if( mSerialTree[i].IsBone ) 
				MayaJointCount++;

			// Check Mesh against all mSerialTree items for a skincluster ('physique') modifier:		
			int IsSkin = ( Object.apiType() == MFn::kMesh) ? 1: 0; //( Object.apiType() == MFn::kMesh ) ? 1 : 0;

			mSerialTree[i].ModifierIdx = -1; // No cluster.			
			int HasCluster = 0;
			if( IsSkin) 
			{
				int Cluster = findSkinCluster( Object);
				HasCluster = ( Cluster >= 0 ) ? 1 : 0;
				if ( Cluster >= 0 ) 
				{
					mSerialTree[i].ModifierIdx = Cluster; //Skincluster index..
				}
			}

			bool SkipThisMesh = false;

			mGeomMeshes += mSerialTree[i].HasMesh;

			//bool SkipThisMesh = ( this->DoSkipSelectedGeom && ((INode*)mSerialTree[i].node)->Selected() );

			if ( IsSkin && !SkipThisMesh  )  // && DoPhysGeom  (HasCluster > -1)
			{
				// SkinCluster/Physique skins dont act as bones.
				mSerialTree[i].IsBone = 0; 
				mSerialTree[i].IsSkin = 1;
				mSerialTree[i].IsSmooth = HasCluster;

				// Multiple physique nodes...
				SkinInf NewSkin;			

				NewSkin.Node = mSerialTree[i].node;
				NewSkin.IsSmoothSkinned = HasCluster;
				NewSkin.IsTextured = 1;
				NewSkin.SceneIndex = i;
				NewSkin.SeparateMesh = 0;

				mOurSkins.push_back(NewSkin);

				// Prefer the first or the selected skin if more than one encountered.
				/*
				if ((((INode*)mSerialTree[i].node)->Selected()) || (PhysiqueNodes == 0))
				{
				OurSkin = mSerialTree[i].node;
				}
				*/

				mPhysiqueNodes++;
			}
			else if (0) // (mSerialTree[i].HasTexture && mSerialTree[i].HasMesh && DoTexGeom  && !SkipThisMesh )
				// Any nonphysique mesh object acts as its own bone ? => never the case for Maya.
			{						
				mSerialTree[i].IsBone = 1; 
				mSerialTree[i].IsSkin = 1;

				// Multiple physique nodes...
				SkinInf NewSkin;			

				NewSkin.Node = mSerialTree[i].node;
				NewSkin.IsSmoothSkinned = 0;
				NewSkin.IsTextured = 1;
				NewSkin.SceneIndex = i;
				NewSkin.SeparateMesh = 1;

				mOurSkins.push_back(NewSkin);
				/*
				// Prefer the first or the selected skin if more than one encountered.
				TSTR NodeName(((INode*)mSerialTree[i].node)->GetName());
				DebugBox("Textured geometry mesh: %s",NodeName);
				*/
			}
		}
	}

	void SceneTranslator::storeNodeTree()
	{
		MStatus stat;

		int NodeNum = 0, ObjectCount = 0;

		clearScenePointers();

		//
		// Iterate over all nodes in the dependency graph. Only some dependency graph nodes are/have actual DAG nodes.
		// 	

		for( MItDependencyNodes DepIt(MFn::kDependencyNode);!DepIt.isDone();DepIt.next())
		{
			MFnDependencyNode	Node(DepIt.item());

			// Get object for this Node.
			MObject	Object = Node.object();

			// Store BlendShapes		
			if ( Object.apiType() == MFn::kBlendShape )
			{
				mAxBlendShapes.append( Object );
			}

			// Maya SkinCluster modifier is not a DAG node - store independently.

			if ( Object.apiType() == MFn::kSkinClusterFilter ) 
			{
				mAxSkinClusters.append( Object );
			}

			if(! DepIt.item().hasFn(MFn::kDagNode))
				continue;

			MFnDagNode DagNode = DepIt.item();

			if(DagNode.isIntermediateObject())
				continue;

			//
			// Simply record all relevant objects.
			// Includes skinclusterfilters(?) (and materials???)
			//

			if( Object != MObject::kNullObj  )
			{
				if( //( Object.hasFn(MFn::kMesh)) ||
					( Object.apiType() == MFn::kMesh ) ||
					( Object.apiType() == MFn::kJoint ) ||
					( Object.apiType() == MFn::kTransform ) )
				{
					mAxSceneObjects.append( Object ); //mAxSceneObjects.AddItem(Object); 

					//if ( ! Object.hasFn(MFn::kDagNode) )
					//	PopupBox("Object %i [%s] has no DAG node",NodeNum, Object.apiTypeStr() );

					if ( Object.apiType() == MFn::kSkinClusterFilter ) 
					{
						//PopupBox("Skincluster found, scenetree item %i name %s",mAxSceneObjects.Num()-1, Node.name(&stat).asChar() );
					}

					NodeInfo NewNode;
					NewNode.node = (void*) ObjectCount;
					NewNode.NumChildren = 0;

					mSerialTree.push_back( NewNode ); // Store index rather than Object.

					ObjectCount++;
				}
				else // Not a mesh or skincluster....
				{
					//DLog.Logf("NonMesh item:%i  Type: [%s]  NodeName: [%s] \r\n",NodeNum, Object.apiTypeStr(), Node.name(&stat).asChar());
				}

			}
			else // Null objects...
			{		
				// DLog.Logf("Null object - Item number: %i   NodeName: [%s] \r\n",NodeNum, Node.name(&stat).asChar() );
			}

			NodeNum++;
		}
	}

	void SceneTranslator::clearScenePointers()
	{
		mAxSceneObjects.clear();
		mAxSkinClusters.clear();	
		mAxShaders.clear();
		mAxBlendShapes.clear();
	}

	int SceneTranslator::findSkinCluster(MObject& obj)
	{
		int SoftSkin = -1;

		for(uint t=0; t<mAxSkinClusters.length(); t++ )
		{
			// 'physique / skincluster modifier' detection		

			// MFnDependencyNode SkinNode = (MFnDependencyNode) SkinObject;
			// PopupBox("Skin search: node name: [%s] ", SkinNode.typeName().asChar() );

			if( mAxSkinClusters[t].apiType() == MFn::kSkinClusterFilter )
			{
				// For each skinCluster node, get the list of influenced objects			
				MFnSkinCluster skinCluster(mAxSkinClusters[t]);
				MDagPathArray infs;
				MStatus stat;
				int nInfs = skinCluster.influenceObjects(infs, &stat);

				//PopupBox("Skincluster found, looking for affected geometry");

				if( nInfs )
				{
					int nGeoms = skinCluster.numOutputConnections();
					for (int ii = 0; ii < nGeoms; ++ii) 
					{
						int index = skinCluster.indexForOutputConnection(ii,&stat);
						//CheckError(stat,"Error getting geometry index.");					

						// Get the dag path of the ii'th geometry.
						MDagPath skinPath;
						stat = skinCluster.getPathAtIndex(index,skinPath);						
						//CheckError(stat,"Error getting geometry path.");

						MObject SkinGeom = skinPath.node();

						// DagNode in skinPath ????					
						if ( SkinGeom == obj ) //TestObjectode == DagNode ) // compare node & node from path
						{
							//PopupBox("Skincluster influence found for -  %s \n", skinPath.fullPathName().asChar() );
							return t;
						}
						// DLog.Logf("Geometry pathname: # [%3i] -  %s \n",ii,skinPath.fullPathName().asChar() );
					}
				}
			}	
		}
		return -1;
	}

	int SceneTranslator::processStaticMesh(int TreeIndex)
	{
		// <Path>.fullPathName().asChar()	
		MStatus	stat;
		MObject MeshObject = mAxSceneObjects[ (int)mSerialTree[TreeIndex].node ]; 
		MFnDagNode DagNode = mAxSceneObjects[ (int)mSerialTree[TreeIndex].node ]; 
		MDagPath  DagPath;
		DagNode.getPath( DagPath );


		//mSerialTree[TreeIndex].IsSelected = isObjectSelected ( DagPath ); 
		mSerialTree[TreeIndex].IsSelected = isMeshSelected ( DagPath );

		if( mDoSelectedStatic && !mSerialTree[TreeIndex].IsSelected )
		{		
			return 0; // Ignore unselected.
		}

		//PCF BEGIN

		////	dirty valid maya triangulation via mel command - soon replaced by api
		////	Suppouse that we need always to triangulate (true?)

		//MStringArray triangNode;
		//MString triangulateCmd = "polyTriangulate -ch 1 " + DagNode.name();
		//MGlobal::executeCommand(triangulateCmd, triangNode, 0, 0);	
		//PCF END

		bool MeshHasMapping = true;
		bool bCollision = false;
		bool bSmoothingGroups = false;

		// Decide if it's a nontextured OR collision-only mesh - look at material.
		MFnMesh MeshFunction( DagPath, &stat );

		int NumVerts = MeshFunction.numVertices();
		int NumFaces = MeshFunction.numPolygons();

		MObjectArray MayaShaders;
		MIntArray ShaderIndices;

		MeshFunction.getConnectedShaders( 0, MayaShaders, ShaderIndices);
		// Get the shader 'index; during triangle processing as with BYTE MaterialIndex = ShaderIndices[TriIndex] & 255;

		int NewMaterials = 0;

		if( MayaShaders.length() == 0) 
		{
			MeshHasMapping = false;		

		}

		// Get name.
		//DLog.Logf(" MESH NAME: [%s] Selected:[%i] mapping: %i \n" , DagNode.name().asChar(), mSerialTree[TreeIndex].IsSelected, (int)MeshHasMapping ); 

		// Recognize any collision primitive naming conventions ?
		// into separate MCD SP BX CY CX_name 
		// MCDCX is the default for untextured geometry.
		char PrimitiveName[MAX_PATH];	
		strcpy_s<MAX_PATH>(PrimitiveName,DagNode.name().asChar());
		//strcpy_s( PrimitiveName, DagNode.name().asChar(), MAX_PATH );	 

		//comment first -tianyi.cheng
		//if( CheckSubString( PrimitiveName,_T("MCD")) ) 
		//{
		//	bCollision = true; // collision-only architecture.
		//}

		// New primitive
		int PrimIndex = mStaticPrimitives.size();
		mStaticPrimitives.push_back(GeometryPrimitive());
		

		mStaticPrimitives[PrimIndex].Name.assign( PrimitiveName );	
		mStaticPrimitives[PrimIndex].Selected = mSerialTree[TreeIndex].IsSelected; 

		//
		//	PCF BEGIN
		//	export in object space 	for t3d file
		//	t3d file for current mesh is exported via mel 
		//
		if (mDoExportInObjectSpace)
		{

			MFnTransform meshTrans (DagPath.transform());
			//MGlobal::displayInfo(meshTrans.name())   ;


			double scals[3] ;
			meshTrans.getScale(scals);

			// pcf: can`t export with scale 
			if (scals[0] != 1 ||scals[1] != 1 ||scals[2] != 1)
			{
				MGlobal::displayError("Object " +meshTrans.name() + " with scale other than 1 1 1 can`t be exported in object space mode. Please freeze scale transformation" );
				return -1;

			}

			MVector trans = meshTrans.getTranslation(MSpace::kPostTransform);


			MEulerRotation  rotation ; 		 
			meshTrans.getRotation(rotation );


			//X = - Points[p].x; 
			//Y =   Points[p].z; 
			//Z =   Points[p].y;

			mStaticPrimitives[PrimIndex]._rotation[0] = rotation.x;
			mStaticPrimitives[PrimIndex]._rotation[1] = rotation.y;
			mStaticPrimitives[PrimIndex]._rotation[2] = rotation.z;


			mStaticPrimitives[PrimIndex]._translation[0] = -trans.x * mVertexExportScale;
			mStaticPrimitives[PrimIndex]._translation[1] = -trans.z * mVertexExportScale;
			mStaticPrimitives[PrimIndex]._translation[2] = trans.y * mVertexExportScale;

		}
		// PCF END



		// Ensure our dagPathMesh is valid..
		if( MS::kSuccess != stat )
			return 0;	

		//change later? -tianyi.cheng
		// If the mesh had no materials/mapping, if desired it will become 'convex' collision architecture automatically ?	
		if( 0 ) // OurScene.DoUntexturedAsCollision && !MeshHasMapping && !bCollision )
		{
			sprintf_s( PrimitiveName, "MCDCX_%s",PrimitiveName );
			bCollision = true;
		}	

		//
		// Now stash the entire thing in our mStaticPrimitives, regardless of mapping/smoothing groups......
		// Accumulate shaders in order of occurrence in the triangles.
		//


		// Get points array & iterate vertices.	
		MDagPath ShapedagPath;
		stat = DagNode.getPath( ShapedagPath );

		// Ensure our dagPathMesh is valid..
		if( MS::kSuccess != stat )
			return 0;

		//PCF BEGIN - object space export + global scale
		MFloatPointArray	Points;		
		MeshFunction.setObject( ShapedagPath );
		if (mDoExportInObjectSpace)
			MeshFunction.getPoints( Points, MSpace::kObject ); 
		else
			MeshFunction.getPoints( Points, MSpace::kWorld ); 

		bool bFlipAxii = MGlobal::isYAxisUp();

		// 3D Vertices.
		if( bFlipAxii )
		{
			for( int p=0; p< NumVerts; p++)
			{
				mStaticPrimitives[PrimIndex].Vertices.push_back(
					Vector3(- Points[p].x * mVertexExportScale,
					Points[p].z* mVertexExportScale,
					Points[p].y* mVertexExportScale));
			}
		}
		else
		{
			for( int p=0; p< NumVerts; p++)
			{
				mStaticPrimitives[PrimIndex].Vertices.push_back(
					Vector3(Points[p].x* mVertexExportScale,
					Points[p].y* mVertexExportScale,
					Points[p].z* mVertexExportScale));

			}
		}
		//PCF END


		// Vertex-color detection.

		bool hasColors = false;
		MString colorSetName;
		MeshFunction.getCurrentColorSetName ( colorSetName);
		MColorArray fColorArray;
		if ( MeshFunction.getCurrentColorSetName ( colorSetName) == MStatus::kSuccess && 
			colorSetName.length() && 
			MeshFunction.getFaceVertexColors(fColorArray) == MStatus::kSuccess) 
		{
			hasColors = true;
		}

		int vertColorCount =fColorArray.length();
		int realColorCount = 0;
		// Any vertex for which color is not defined will have -1 in all its components.
		for( int i=0; i<vertColorCount;i++)
		{
			if( ! (
				(fColorArray[i].r == -1.f ) ||
				(fColorArray[i].g == -1.f ) ||
				(fColorArray[i].b == -1.f ) 
				) )
				realColorCount++;
		}
		// PopupBox(" Vertex colors - bulk  %i  - real: %i  for (sub) mesh %s ",vertColorCount,realColorCount,MeshFunction.name().asChar());





		//PCF BEGIN
		// maya api triangulation
#ifdef NEWMAYAEXPORT
		if( MayaShaders.length() == 0) 
		{
			MGlobal::displayWarning(MString("possible shader-less geometry encountered for node  ") + DagNode.name());
		}

		triangulatedMesh triMesh;
		if (! triMesh.triangulate(DagPath))
		{
			MGlobal::displayError(MString("triangulation failed for  ") + DagNode.name());
			return -1;

		}
		triMesh.createEdgesPool();

		// Any smoothing group conversion requested ?
		MeshProcessor TempTranslator;
		if( mDoConvertSmooth && MeshHasMapping && ( !bCollision ) )
		{
			TempTranslator.createNewSmoothingGroupsFromTriangulatedMesh( triMesh );  // NEW 3dsmax-type-smoothing group creation.

			// Copy smoothing groups over to the primitive's own.
			if( TempTranslator.mFaceSmoothingGroups.size()  ) 
			{
				bSmoothingGroups = true;
				mStaticPrimitives[PrimIndex].mFaceSmoothingGroups.clear();
				mStaticPrimitives[PrimIndex].mFaceSmoothingGroups.AddExactZeroed(  TempTranslator.mFaceSmoothingGroups.Num() );
				for( int f=0; f< TempTranslator.mFaceSmoothingGroups.size(); f++)
				{				
					for( int s=0; s< TempTranslator.mFaceSmoothingGroups[f].Groups.size(); s++)
					{
						mStaticPrimitives[PrimIndex].mFaceSmoothingGroups[f].Groups.push_back(  TempTranslator.mFaceSmoothingGroups[f].Groups[s] );
					}
				}			
			}
		}
		std::vector< GroupList > NewSmoothingGroups;
		bool bHasSmoothingGroups = (mStaticPrimitives[PrimIndex].mFaceSmoothingGroups.size() > 0);


		for ( int i =0 ; i <  triMesh.triangles.size(); i++)
		{
			triangulatedMesh::Triangle & t = *(triMesh.triangles[i]);
			int PolyIndex = i;


			int MaterialIndex = t.ShadingGroupIndex & 255;

			int ThisMaterial = 0;
			if(MaterialIndex == -1)
			{
				MGlobal::displayError(DagPath.partialPathName() + MString(".f[") + i + MString("doesn`t have material assigned!" ));
				return  -1 ;
			}
			if( (int)MayaShaders.length() <= MaterialIndex ) 
			{			
				ThisMaterial = 0;
			}
			else		
			{
				int OldShaderNum = mAxShaders.length();
				ThisMaterial = DigestMayaMaterial( MayaShaders[MaterialIndex] );

				// New material ?
				if( (int)mAxShaders.length() > OldShaderNum )
					NewMaterials++;
			}


			//// A brand new face.

			int NewFaceIdx = mStaticPrimitives[PrimIndex].Faces.AddZeroed(1); 
			mStaticPrimitives[PrimIndex].Faces[NewFaceIdx].MaterialIndices.AddExactZeroed(1);
			mStaticPrimitives[PrimIndex].Faces[NewFaceIdx].MaterialIndices[0] = ThisMaterial;

			if( bHasSmoothingGroups )
			{
				//  Necessary for redistribution of smoothing groups for faces with more than 3 vertices being auto-triangulated.			
				NewSmoothingGroups.AddZeroed(1);
				for( int s=0; s< mStaticPrimitives[PrimIndex].mFaceSmoothingGroups[PolyIndex].Groups.size(); s++)
				{
					NewSmoothingGroups[NewFaceIdx].Groups.push_back( mStaticPrimitives[PrimIndex].mFaceSmoothingGroups[PolyIndex].Groups[s] );
				}
			}

			for ( int j =0 ; j <3; j++)
			{
				int VertIdx = t.Indices[j].pointIndex;

				GWedge NewWedge;				
				//int NewWedgeIdx = mStaticPrimitives[PrimIndex].Wedges.AddZeroed(1);
				NewWedge.MaterialIndex = ThisMaterial; // Per-corner material index..
				NewWedge.PointIndex = VertIdx; // Maya's face vertices indices for this face.

				int indexuv =0;

				int uvIndex = t.Indices[j].tcIndex[0];
				NewWedge.U = triMesh.uArray[0][uvIndex];
				NewWedge.V = triMesh.vArray[0][uvIndex];



				// Should we merge identical wedges here instead of counting on editor ?  With the way ASE is imported it may make no difference.
				int NewWedgeIdx = mStaticPrimitives[PrimIndex].Wedges.size();
				mStaticPrimitives[PrimIndex].Wedges.push_back( NewWedge);
				// New wedge on every corner of a face. 
				mStaticPrimitives[PrimIndex].Faces[NewFaceIdx].WedgeIndex[j] = NewWedgeIdx; 



				if (t.Indices[j].hasExtraUVs[0])
				{
					uvIndex = t.Indices[j].tcIndex[1];

					GWedge NewWedge2;				
					NewWedge2.MaterialIndex = ThisMaterial; 
					NewWedge2.U = triMesh.uArray[1][uvIndex];
					NewWedge2.V = triMesh.vArray[1][uvIndex];
					NewWedge2.PointIndex = VertIdx;

					int NewWedge2Idx = mStaticPrimitives[PrimIndex].Wedges2.size();
					mStaticPrimitives[PrimIndex].Wedges2.push_back( NewWedge2);
					mStaticPrimitives[PrimIndex].Faces[NewFaceIdx].Wedge2Index[j] = NewWedge2Idx;  


				}

				// Store per-vertex color (for this new wedge) only if any were actually defined. 
				if( hasColors &&  realColorCount > 0 )
				{
					GColor NewVertColor; // Defaults to RGBA = 0.0 0.0 0.0 1.0
					int colorIndex = t.Indices[j].vertexColor;

					if( (colorIndex >= 0) && (colorIndex < (int)fColorArray.length()) )
					{
						NewVertColor.A = fColorArray[colorIndex].a;
						NewVertColor.R = fColorArray[colorIndex].r;
						NewVertColor.G = fColorArray[colorIndex].g;
						NewVertColor.B = fColorArray[colorIndex].b;
					}

					// Store in VertColors - which will be either empty OR contain the same number of elements as this primitive's Wedges array.
					mStaticPrimitives[PrimIndex].VertColors.push_back(NewVertColor);
				}

			}
		}

		//PCF END

#else
		// old export code

		// Any smoothing group conversion requested ?

		MeshProcessor TempTranslator;

		if( OurScene.DoConvertSmooth && MeshHasMapping && ( !bCollision ) )
		{
			MFnDagNode fnMeshNode( MeshObject, &stat); // Intermediate to get at the DAGPATH..
			if( MS::kSuccess == stat)
			{
				MDagPath dagPathMesh;
				stat = fnMeshNode.getPath( dagPathMesh );  // Puts DagPath in dagPathMesh.			
				//TempTranslator.buildEdgeTable( dagPathMesh ); 				 OLD smoothing group conversion..
				TempTranslator.createNewSmoothingGroups( dagPathMesh );  // NEW 3dsmax-type-smoothing group creation.
			}

			// Copy smoothing groups over to the primitive's own.
			if( TempTranslator.mFaceSmoothingGroups.Num()  ) 
			{
				bSmoothingGroups = true;
				mStaticPrimitives[PrimIndex].mFaceSmoothingGroups.clear();
				mStaticPrimitives[PrimIndex].mFaceSmoothingGroups.AddExactZeroed(  TempTranslator.mFaceSmoothingGroups.Num() );
				for( int f=0; f< TempTranslator.mFaceSmoothingGroups.Num(); f++)
				{				
					for( int s=0; s< TempTranslator.mFaceSmoothingGroups[f].Groups.Num(); s++)
					{
						mStaticPrimitives[PrimIndex].mFaceSmoothingGroups[f].Groups.AddItem(  TempTranslator.mFaceSmoothingGroups[f].Groups[s] );
					}
				}			
			}
		}
		TArray< GroupList > NewSmoothingGroups;
		bool bHasSmoothingGroups = (mStaticPrimitives[PrimIndex].mFaceSmoothingGroups.Num() > 0);

		// UV set tally.
		MStringArray uvSetNames;
		MeshFunction.getUVSetNames(uvSetNames);
		int uvSetCount = uvSetNames.length();
		MString firstUVSetName = MString("");
		MString secondUVSetName = MString("");
		if( uvSetCount > 0)
		{
			firstUVSetName = uvSetNames[0];
		}
		if( uvSetCount > 1)
		{
			secondUVSetName = uvSetNames[1];
		}

		// Faces & Wedges & Materials & Smoothing groups, all in the same run......
		for (int PolyIndex = 0; PolyIndex < NumFaces; PolyIndex++)
		{				
			// Get the vertex indices for this polygon.
			MIntArray	FaceVertices;		
			MeshFunction.getPolygonVertices(PolyIndex,FaceVertices);
			int VertCount = FaceVertices.length();

			// Assumed material the same for all facets of a poly.		
			// Material on this face - encountered before ? -> DigestMayaMaterial..
			int MaterialIndex = ShaderIndices[PolyIndex]; // & 255;
			if(MaterialIndex == -1)
			{
				MString _aaa = "Some polys don`t have materials on object \"";
				_aaa += DagNode.name();
				_aaa += "\"";


				MGlobal::displayError(_aaa );
				cerr << "ERROR: " << _aaa.asChar() << endl;

				return  -1 ;

			}
			int ThisMaterial = 0;


			// Only count material on valid polygons.
			if( VertCount >= 3)
			{
				if( (int)MayaShaders.length() <= MaterialIndex ) 
				{			
					ThisMaterial = 0;
				}
				else		
				{
					int OldShaderNum = AxShaders.length();
					ThisMaterial = DigestMayaMaterial( MayaShaders[MaterialIndex] );

					// New material ?
					if( (int)AxShaders.length() > OldShaderNum )
						NewMaterials++;
				}
				//DLog.LogfLn(" Material for poly %i is %i total %i ",PolyIndex,ThisMaterial,AxShaders.length()); 
			}

			// Handle facets of single polygon.
			while( VertCount >= 3 )
			{							
				// A brand new face.
				int NewFaceIdx = mStaticPrimitives[PrimIndex].Faces.AddZeroed(1); 
				mStaticPrimitives[PrimIndex].Faces[NewFaceIdx].MaterialIndices.AddExactZeroed(1);
				mStaticPrimitives[PrimIndex].Faces[NewFaceIdx].MaterialIndices[0] = ThisMaterial;

				if( bHasSmoothingGroups )
				{
					//  Necessary for redistribution of smoothing groups for faces with more than 3 vertices being auto-triangulated.			
					NewSmoothingGroups.AddZeroed(1);
					for( int s=0; s< mStaticPrimitives[PrimIndex].mFaceSmoothingGroups[PolyIndex].Groups.Num(); s++)
					{
						NewSmoothingGroups[NewFaceIdx].Groups.AddItem( mStaticPrimitives[PrimIndex].mFaceSmoothingGroups[PolyIndex].Groups[s] );
					}
				}

				//DLog.LogfLn(" Material on facet %i Face %i Primitive %i is %i NumMaterialsForface %i DATA %i ",VertCount,NewFaceIdx,PrimIndex,ThisMaterial,mStaticPrimitives[PrimIndex].Faces[NewFaceIdx].MaterialIndices.Num(), (int)((BYTE*)&mStaticPrimitives[PrimIndex].Faces[NewFaceIdx].MaterialIndices[0]) );			

				// Fill vertex indices (breaks up face in triangle polygons.
				int VertIdx[3];
				VertIdx[0] = 0;
				VertIdx[1] = VertCount-2;
				VertIdx[2] = VertCount-1;

				for( int i=0; i<3; i++)
				{
					//Retrieve wedges for first UV set.
					float U,V;								
					stat = MeshFunction.getPolygonUV( PolyIndex,VertIdx[i],U,V,&firstUVSetName);
					if ( stat != MS::kSuccess )
					{
						DLog.Logf(" Invalid UV retrieval, index [%i] for face [%i]", VertIdx[i], PolyIndex ); 
					}

					//DLog.Logf(" UV logging: Face: %6i Index %6i (%6i) U %6f  V %6f \n",NewFaceIdx,i,VertIdx[i],U,V); 

					GWedge NewWedge;				
					//int NewWedgeIdx = mStaticPrimitives[PrimIndex].Wedges.AddZeroed(1);

					NewWedge.MaterialIndex = ThisMaterial; // Per-corner material index..
					NewWedge.U = U;
					NewWedge.V = V;
					NewWedge.PointIndex = FaceVertices[VertIdx[i]]; // Maya's face vertices indices for this face.

					// Should we merge identical wedges here instead of counting on editor ?  With the way ASE is imported it may make no difference.
					int NewWedgeIdx = mStaticPrimitives[PrimIndex].Wedges.AddItem( NewWedge);
					// New wedge on every corner of a face. 
					mStaticPrimitives[PrimIndex].Faces[NewFaceIdx].WedgeIndex[i] = NewWedgeIdx; 

					// Any second UV set data ?
					if( uvSetCount > 1)
					{
						float U,V;								
						stat = MeshFunction.getPolygonUV( PolyIndex,VertIdx[i],U,V,&secondUVSetName);
						if ( stat != MS::kSuccess )
						{
							DLog.Logf(" Invalid secondary UV retrieval, index [%i] for face [%i]", VertIdx[i], PolyIndex ); 
						}
						GWedge NewWedge2;				
						NewWedge2.MaterialIndex = ThisMaterial; 
						NewWedge2.U = U;
						NewWedge2.V = V;
						NewWedge2.PointIndex = FaceVertices[VertIdx[i]];

						int NewWedge2Idx = mStaticPrimitives[PrimIndex].Wedges2.AddItem( NewWedge2);
						mStaticPrimitives[PrimIndex].Faces[NewFaceIdx].Wedge2Index[i] = NewWedge2Idx;  
					}

					// Store per-vertex color (for this new wedge) only if any were actually defined. 
					if( realColorCount > 0 )
					{
						GColor NewVertColor; // Defaults to RGBA = 0.0 0.0 0.0 1.0
						int colorIndex = -1;
						if (MStatus::kFailure == MeshFunction.getFaceVertexColorIndex( PolyIndex,VertIdx[i], colorIndex)) 
						{
							//MGlobal::displayError("MFnMesh::getFaceVertexColorIndex");
							//return MStatus::kFailure;
						}
						else
						{
							if( (colorIndex >= 0) && (colorIndex < (int)fColorArray.length()) )
							{
								NewVertColor.A = fColorArray[colorIndex].a;
								NewVertColor.R = fColorArray[colorIndex].r;
								NewVertColor.G = fColorArray[colorIndex].g;
								NewVertColor.B = fColorArray[colorIndex].b;
							}
						}		
						// Store in VertColors - which will be either empty OR contain the same number of elements as this primitive's Wedges array.
						mStaticPrimitives[PrimIndex].VertColors.AddItem(NewVertColor);
					}
				}	

				VertCount--;
			}

		}// For each primitive..	


#endif	
		if( bHasSmoothingGroups )
		{
			// Copy back the newly distributed smoothing groups.
			mStaticPrimitives[PrimIndex].mFaceSmoothingGroups.clear();
			mStaticPrimitives[PrimIndex].mFaceSmoothingGroups.AddExactZeroed(NewSmoothingGroups.Num());
			for( int f=0; f< NewSmoothingGroups.size(); f++)
			{		
				for( int s=0; s<NewSmoothingGroups[f].Groups.size(); s++)
				{
					mStaticPrimitives[PrimIndex].mFaceSmoothingGroups[f].Groups.push_back( NewSmoothingGroups[f].Groups[s] ); 				
				}
			}
		}

		//DLog.Logf(" Primitive [%s] processed - Faces %i verts %i UVpairs %i  New Materials %i \n", 
		//	mStaticPrimitives[PrimIndex].Name.StringPtr(),
		//	mStaticPrimitives[PrimIndex].Faces.Num(), 
		//	mStaticPrimitives[PrimIndex].Vertices.Num(),
		//	mStaticPrimitives[PrimIndex].Wedges.Num(),
		//	NewMaterials
		//	);

		////pcf: remove triangulations
		//for (int q =0; q< triangNode.length() ; q++)
		//{
		//	triangulateCmd = "catchQuiet(`delete " + triangNode[q] + "`)";	
		//	MGlobal::executeCommand(triangulateCmd, triangNode[q], 0, 0);
		//}
		return 1;
	}

	bool SceneTranslator::isObjectSelected(const MDagPath& path)
	{
		MStatus status;
		MDagPath sDagPath;

		MSelectionList activeList;
		MGlobal::getActiveSelectionList( activeList );

		MItSelectionList iter ( activeList, MFn::kDagNode );

		while ( !iter.isDone() )
		{
			if ( iter.getDagPath( sDagPath ) )
			{
				//MString PathOne = sDagPath.fullPathName(&status);
				//MString PathTwo = path.fullPathName(&status);
				//DLog.LogfLn(" Selection check: [%s] against <[%s]>",PathOne.asChar(), PathTwo.asChar() );

				if ( sDagPath == path )
					return true;
			}
			iter.next();
		}

		return false;
	}

	bool SceneTranslator::isMeshSelected(const MDagPath& path)
	{
		MStatus status;

		//create an iterator for the selected mesh components of the DAG
		MSelectionList selectionList;
		if (MStatus::kFailure == MGlobal::getActiveSelectionList(selectionList)) 
		{			
			return false;
		}

		MItSelectionList itSelectionList(selectionList, MFn::kMesh, &status);	
		if (MStatus::kFailure == status) 
		{
			return false;
		}

		for (itSelectionList.reset(); !itSelectionList.isDone(); itSelectionList.next()) 
		{
			MDagPath dagPath;
			//get the current dag path 
			if( MStatus::kFailure != itSelectionList.getDagPath(dagPath)) 
			{   			
				//MString PathOne = dagPath.fullPathName(&status);
				//MString PathTwo = path.fullPathName(&status);
				//DLog.LogfLn(" Selection check: [%s] against <[%s]>",PathOne.asChar(), PathTwo.asChar() );

				if( path == dagPath )
					return true;
			}
		}
		return false;
	}
}