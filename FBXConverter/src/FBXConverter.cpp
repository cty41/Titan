#include "FBXConverter.h"
#include <vector>

using namespace Titan;

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(mpSdkMgr->GetIOSettings()))
#endif

Vector3 ConvertPos(const KFbxVector4& Pos)
{
	Vector3 out;
	out[0] = Pos[0];
	// flip Y, then the right-handed axis system is converted to LHS
	out[1] = -Pos[1];
	out[2] = Pos[2];

	return out;
}

Vector3 ConvertNormal(const KFbxVector4& normal)
{
	Vector3 out;
	out[0] = normal[0];
	out[1] = normal[1];
	out[2] = normal[2];

	return out;
}


//the idx of the insertion, if the vector already has a val just return the idx
template<class T>
int AddUniqueItem(std::vector<T>& vec, const T& val)
{
	size_t num = 0;
	for(auto it = vec.begin();it != vec.end();++it, ++num)
	{
		if((*it) == val)
			return num;
	}
	vec.push_back(val);
	return vec.size() - 1;
}



FBXConverter::FBXConverter(const AcceptArgs& args)
	:mpSdkMgr(NULL), mpScene(NULL), mConvertAll(args.convertAll), mConvertAscii(args.convertAscii), mInFileName(args.src_file), mOutFileName(args.dst_file),
	mpAsciiFile(nullptr)
{
	InitializeSdkObjects();

	bool stat = true;

	if(mConvertAll)
	{
		//todo
	}
	else
	{
		stat = LoadScene(mInFileName);
		if(!stat)
		{
			printf("An error occurred while loading the scene...\n");
		}
		else
		{
			stat = ExportScene(mOutFileName);
			if(!stat)
				printf("Export process failed! \n");
		}
	}
}

FBXConverter::~FBXConverter()
{
	if(mpScene)
	{
		mpScene->Destroy();
		mpScene = nullptr;
	}

	DestroySdkObjects();
}

void FBXConverter::InitializeSdkObjects()
{
	mpSdkMgr = KFbxSdkManager::Create();

	if (!mpSdkMgr)
	{
		printf("Unable to create the FBX SDK manager\n");
		exit(0);
	}

	// create an IOSettings object
	KFbxIOSettings * ios = KFbxIOSettings::Create(mpSdkMgr, IOSROOT );
	mpSdkMgr->SetIOSettings(ios);

	// Load plugins from the executable directory
	KString lPath = KFbxGetApplicationDirectory();
	KString lExtension = "dll";
	mpSdkMgr->LoadPluginsDirectory(lPath.Buffer(), lExtension.Buffer());

	// Create the entity that will hold the scene.
	mpScene = KFbxScene::Create(mpSdkMgr,"");

	mFbxGeometryConverter = new KFbxGeometryConverter(mpSdkMgr);
}

void FBXConverter::DestroySdkObjects()
{
	if(mpSdkMgr)
	{
		mpSdkMgr->Destroy();
		mpSdkMgr = NULL;
	}

	if(mFbxGeometryConverter)
	{
		delete mFbxGeometryConverter;
		mFbxGeometryConverter = NULL;
	}
}
/*
*	load KFbxScene Object
*/
bool FBXConverter::LoadScene(const std::string& infile)
{
	//clear previous scene first
	mTiScene.ClearScene();


	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor,  lSDKMinor,  lSDKRevision;
	//int lFileFormat = -1;
	int i, lAnimStackCount;
	bool lStatus;
	char lPassword[1024];

	// Get the file version number generate by the FBX SDK.
	KFbxSdkManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	KFbxImporter* lImporter = KFbxImporter::Create(mpSdkMgr,"");
	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(mInFileName.c_str(), -1, mpSdkMgr->GetIOSettings());
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if( !lImportStatus )
	{
		printf("Call to KFbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetLastErrorString());

		if (lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_YET ||
			lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_ANYMORE)
		{
			printf("FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
			printf("FBX version number for file %s is %d.%d.%d\n\n", mInFileName.c_str(), lFileMajor, lFileMinor, lFileRevision);
		}

		return false;
	}

	printf("FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX())
	{
		printf("FBX version number for file %s is %d.%d.%d\n\n", mInFileName.c_str(), lFileMajor, lFileMinor, lFileRevision);

		// From this point, it is possible to access animation stack information without
		// the expense of loading the entire file.

		printf("Animation Stack Information\n");

		lAnimStackCount = lImporter->GetAnimStackCount();

		printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
		printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
		printf("\n");

		for(i = 0; i < lAnimStackCount; i++)
		{
			KFbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

			printf("    Animation Stack %d\n", i);
			printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
			printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

			// Change the value of the import name if the animation stack should be imported 
			// under a different name.
			printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

			// Set the value of the import state to false if the animation stack should be not
			// be imported. 
			printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
			printf("\n");
		}

		// Set the import states. By default, the import states are always set to 
		// true. The code below shows how to change these states.
		IOS_REF.SetBoolProp(IMP_FBX_MATERIAL,        true);
		IOS_REF.SetBoolProp(IMP_FBX_TEXTURE,         true);
		IOS_REF.SetBoolProp(IMP_FBX_LINK,            true);
		IOS_REF.SetBoolProp(IMP_FBX_SHAPE,           true);
		IOS_REF.SetBoolProp(IMP_FBX_GOBO,            true);
		IOS_REF.SetBoolProp(IMP_FBX_ANIMATION,       true);
		IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}

	// Import the scene.
	lStatus = lImporter->Import(mpScene);

	if(lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)
	{
		printf("Please enter password: ");

		lPassword[0] = '\0';

		scanf_s("%s", lPassword);
		KString lString(lPassword);

		IOS_REF.SetStringProp(IMP_FBX_PASSWORD,      lString);
		IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

		lStatus = lImporter->Import(mpScene);

		if(lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)
		{
			printf("\nPassword is wrong, import aborted.\n");
		}
	}

	// Destroy the importer.
	lImporter->Destroy();

	//now convert fbx data structure into our custom format
	ConvertContent();
}

void FBXConverter::ConvertContent()
{
	KFbxNode* pRootNode = mpScene->GetRootNode();

	if(pRootNode != NULL)
	{
		for(int i = 0; i < pRootNode->GetChildCount(); ++i)
			ConvertNode(pRootNode->GetChild(i));
	}
}

void FBXConverter::ConvertNode(KFbxNode* pNode)
{
	KFbxNodeAttribute::EAttributeType lAttributeType;
	lAttributeType = pNode->GetNodeAttribute()->GetAttributeType();
	switch(lAttributeType)
	{
	case KFbxNodeAttribute::eMESH: 
		ConvertMeshData(pNode);
		break;
		//todo , add skeleton support
	case KFbxNodeAttribute::eSKELETON:
		ConvertSkeletonData(pNode);
		break;
	default: break;
	}

	for(int i = 0; i < pNode->GetChildCount(); ++i)
		ConvertNode(pNode->GetChild(i));
}

void FBXConverter::ConvertMeshData(KFbxNode* pNode)
{
	KFbxNodeAttribute* pNodeAtt = pNode->GetNodeAttribute();
	KFbxMesh* fbxMesh = pNode->GetMesh();

	//remove the bad polygons before getting any data from mesh
	fbxMesh->RemoveBadPolygons();

	//triangle mesh
	if(!fbxMesh->IsTriangleMesh())
		fbxMesh = mFbxGeometryConverter->TriangulateMesh(fbxMesh);

	TiSubMesh*	tSubMesh = new TiSubMesh(&mTiScene, fbxMesh->GetName());
	mTiScene.mSubMeshes.push_back(tSubMesh);

	//Get the base layer of the mesh
	KFbxLayer* BaseLayer = fbxMesh->GetLayer(0);
	if (BaseLayer == NULL)
	{
		printf("There is no Geometry infos in mesh!\n");
		return;
	}
	tSubMesh->mVertexDecls.push_back(TiVertexDecl(kVDS_POS, 0, 4 * 3));

	/*
	*	store uvSets name in array for later use
	*/
	int layerCount = fbxMesh->GetLayerCount();
	std::vector<std::string> UVSets;
	if(layerCount > 0)
	{
		for(int layerIdx = 0;layerIdx < layerCount;++layerIdx)
		{
			KFbxLayer* layer = fbxMesh->GetLayer(0);
			int uvSetCount = layer->GetUVSetCount();
			if (uvSetCount)
			{
				KArrayTemplate<KFbxLayerElementUV const*> EleUVs = layer->GetUVSets();
				for(int uvIdx = 0; uvIdx < uvSetCount;++uvIdx)
				{
					KFbxLayerElementUV const* elementUV = EleUVs[uvIdx];
					if(elementUV)
					{
						std::string uvSetName = elementUV->GetName();
						AddUniqueItem(UVSets, uvSetName);
					}
				}
			}
		}
	}

	int uniqueUVCount = UVSets.size();
	KFbxLayerElementUV** layerElementUV = NULL;
	KFbxLayerElement::EReferenceMode* UVReferenceMode = NULL;
	KFbxLayerElement::EMappingMode* UVMappingMode = NULL;
	if (uniqueUVCount > 0)
	{
		layerElementUV = new KFbxLayerElementUV*[uniqueUVCount];
		UVReferenceMode = new KFbxLayerElement::EReferenceMode[uniqueUVCount];
		UVMappingMode = new KFbxLayerElement::EMappingMode[uniqueUVCount];
		//add uv  (2 * 4 bytes) decl
		for(int i = 0;i < uniqueUVCount;++i)
			tSubMesh->mVertexDecls.push_back(TiVertexDecl(kVDS_TEXCOORD, i, 4 * 2));
	}

	for (int uvIndex = 0; uvIndex < uniqueUVCount; uvIndex++)
	{
		bool bFoundUV = false;
		for (int UVLayerIndex = 0; !bFoundUV && UVLayerIndex<layerCount; UVLayerIndex++)
		{
			KFbxLayer* lLayer = fbxMesh->GetLayer(UVLayerIndex);
			int UVSetCount = lLayer->GetUVSetCount();
			if(UVSetCount)
			{
				KArrayTemplate<KFbxLayerElementUV const*> EleUVs = lLayer->GetUVSets();
				for (int FbxUVIndex = 0; FbxUVIndex<UVSetCount; FbxUVIndex++)
				{
					KFbxLayerElementUV const* ElementUV = EleUVs[FbxUVIndex];
					if (ElementUV)
					{
						std::string UVSetName = ElementUV->GetName();
						if (UVSetName == UVSets.at(uvIndex))
						{
							layerElementUV[uvIndex] = const_cast<KFbxLayerElementUV*>(ElementUV);
							UVReferenceMode[uvIndex] = layerElementUV[FbxUVIndex]->GetReferenceMode();
							UVMappingMode[uvIndex] = layerElementUV[FbxUVIndex]->GetMappingMode();
							break;
						}
					}
				}
			}
		}
	}

	/*
	*	smoothing group layer
	*/
	bool bSmoothingGroup = false;
	KFbxLayerElementSmoothing const* SmoothingInfo = BaseLayer->GetSmoothing();
	KFbxLayerElement::EReferenceMode SmoothingReferenceMode(KFbxLayerElement::eDIRECT);
	KFbxLayerElement::EMappingMode SmoothingMappingMode(KFbxLayerElement::eBY_EDGE);

	if(SmoothingInfo)
	{
		if(SmoothingInfo->GetMappingMode() == KFbxLayerElement::eBY_EDGE)
		{
			if(mFbxGeometryConverter->ComputePolygonSmoothingFromEdgeSmoothing(fbxMesh))
			{
				printf("convert smoothing group failed\n");
			}
		}

		if(SmoothingInfo->GetMappingMode() == KFbxLayerElement::eBY_POLYGON)
			bSmoothingGroup = true;

		SmoothingReferenceMode = SmoothingInfo->GetReferenceMode();
		SmoothingMappingMode = SmoothingInfo->GetMappingMode();
	}

	//
	// get the first vertex color layer
	//
	KFbxLayerElementVertexColor* LayerElementVertexColor = BaseLayer->GetVertexColors();
	KFbxLayerElement::EReferenceMode VertexColorReferenceMode(KFbxLayerElement::eDIRECT);
	KFbxLayerElement::EMappingMode VertexColorMappingMode(KFbxLayerElement::eBY_CONTROL_POINT);
	if (LayerElementVertexColor)
	{
		VertexColorReferenceMode = LayerElementVertexColor->GetReferenceMode();
		VertexColorMappingMode = LayerElementVertexColor->GetMappingMode();

		//unsigned int 32bit
		tSubMesh->mVertexDecls.push_back(TiVertexDecl(kVDS_COLOR, 0, 4));
	}

	//
	// get the first normal layer
	//
	KFbxLayerElementNormal* LayerElementNormal = BaseLayer->GetNormals();
	KFbxLayerElementTangent* LayerElementTangent = BaseLayer->GetTangents();
	KFbxLayerElementBinormal* LayerElementBinormal = BaseLayer->GetBinormals();

	//whether there is normal, tangent and binormal data in this mesh
	bool bHasNTBInformation = LayerElementNormal && LayerElementTangent && LayerElementBinormal;

	KFbxLayerElement::EReferenceMode NormalReferenceMode(KFbxLayerElement::eDIRECT);
	KFbxLayerElement::EMappingMode NormalMappingMode(KFbxLayerElement::eBY_CONTROL_POINT);
	if (LayerElementNormal)
	{
		NormalReferenceMode = LayerElementNormal->GetReferenceMode();
		NormalMappingMode = LayerElementNormal->GetMappingMode();
		//normal decl 3 * 4 bytes
		tSubMesh->mVertexDecls.push_back(TiVertexDecl(kVDS_NORMAL, 0, 3 * 4));
	}

	KFbxLayerElement::EReferenceMode TangentReferenceMode(KFbxLayerElement::eDIRECT);
	KFbxLayerElement::EMappingMode TangentMappingMode(KFbxLayerElement::eBY_CONTROL_POINT);
	if (LayerElementTangent)
	{
		TangentReferenceMode = LayerElementTangent->GetReferenceMode();
		TangentMappingMode = LayerElementTangent->GetMappingMode();
	}

	/*
	*	build mesh triangles
	*/
	// Construct the matrices for the conversion from right handed to left handed system
	KFbxXMatrix TotalMatrix;
	KFbxXMatrix TotalMatrixForNormal;
	TotalMatrix = ComputeTotalMatrix(pNode);
	TotalMatrixForNormal = TotalMatrix.Inverse().Transpose();

	KFbxXMatrix LeftToRightMatrix;
	KFbxXMatrix LeftToRightMatrixForNormal;
	LeftToRightMatrix.SetS(KFbxVector4(1.0, -1.0, 1.0));
	LeftToRightMatrixForNormal = LeftToRightMatrix.Inverse();
	LeftToRightMatrixForNormal = LeftToRightMatrixForNormal.Transpose();

	int triCount = fbxMesh->GetPolygonCount();
	std::vector<StaticMeshTriangle> RawTriangles;
	RawTriangles.reserve(triCount);
	int triangleIdx;
	for(triangleIdx = 0; triangleIdx < triCount; ++triangleIdx)
	{
		//StaticMeshTriangle* triangle = &(RawTriangles.at(triangleIdx));
		StaticMeshTriangle triangle;
		int vertexIdx;
		for(vertexIdx = 0; vertexIdx < 3;++vertexIdx)
		{
			/*
			*	Vertex Position
			*/
			int controlPointIdx = fbxMesh->GetPolygonVertex(triangleIdx, vertexIdx);
			KFbxVector4 fbxPos = fbxMesh->GetControlPoints()[controlPointIdx];
			KFbxVector4 finalPos = TotalMatrix.MultT(fbxPos);
			triangle.Vertices[vertexIdx].Position = ConvertPos(finalPos);
			/*
			*	Vertex Color
			*/
			if(LayerElementVertexColor)
			{
				int VertexColorMappingIdx = (VertexColorMappingMode == KFbxLayerElement::eBY_CONTROL_POINT)?
					fbxMesh->GetPolygonVertex(triangleIdx, vertexIdx) : (triangleIdx * 3 + vertexIdx);

				int VertexColorIdx = (VertexColorReferenceMode == KFbxLayerElement::eDIRECT)? 
					VertexColorMappingIdx : LayerElementVertexColor->GetIndexArray().GetAt(VertexColorMappingIdx);

				KFbxColor vColor = LayerElementVertexColor->GetDirectArray().GetAt(VertexColorIdx);

				Titan::Color tiColor = Titan::Color(vColor.mRed, vColor.mGreen, vColor.mBlue, vColor.mAlpha);
				triangle.Vertices[vertexIdx].Color = tiColor.getAsRGBA();
			}
			/*
			*	UVs
			*/
			for(int uvLayerIdx = 0; uvLayerIdx < uniqueUVCount;++uvLayerIdx)
			{
				if(layerElementUV[uvLayerIdx] != NULL)
				{
					int controlPointIdx = fbxMesh->GetPolygonVertex(triangleIdx, vertexIdx);
					int uvMapIdx = (UVMappingMode[uvLayerIdx] == KFbxLayerElement::eBY_CONTROL_POINT)?
									controlPointIdx : triangleIdx * 3 + vertexIdx;
					int uvIdx = (UVReferenceMode[uvLayerIdx] == KFbxLayerElement::eDIRECT)? 
									uvMapIdx: layerElementUV[uvLayerIdx]->GetIndexArray().GetAt(uvMapIdx);
					KFbxVector2 uvVector = layerElementUV[uvLayerIdx]->GetDirectArray().GetAt(uvIdx);

					Titan::Vector2 uv;
					uv.x = (float)uvVector[0];
					//flip V for directX
					uv.y = 1.0f - (float)uvVector[1];
					triangle.Vertices[vertexIdx].UVs.push_back(uv);
				}
			}
			/*
			*	normals
			*/
			//if(LayerElementNormal)
			//{
			//	int normalMapIdx = (NormalMappingMode == KFbxLayerElement::eBY_CONTROL_POINT)?
			//		vertexIdx : triangleIdx * 3 + vertexIdx;
			//	int normalValIdx = (NormalReferenceMode == KFbxLayerElement::eDIRECT)?
			//		normalMapIdx : LayerElementNormal->GetIndexArray().GetAt(normalMapIdx);

			//	KFbxVector4 fbxNormal = LayerElementNormal->GetDirectArray().GetAt(normalValIdx);
			//	fbxNormal = TotalMatrixForNormal.MultT(fbxNormal);
			//	fbxNormal = LeftToRightMatrixForNormal.MultT(fbxNormal);
			//	triangle.Vertices[vertexIdx].Normal = ConvertNormal(fbxNormal);
			//}
		}
		RawTriangles.push_back(triangle);
	}

	for(int triangleIdx = 0;triangleIdx < triCount; ++triangleIdx)
	{
		StaticMeshTriangle* triangle = &(RawTriangles.at(triangleIdx));

		for(int vertexIdx = 0; vertexIdx < 3;++vertexIdx)
		{
			int index = AddUniqueItem(tSubMesh->mVertexData, triangle->Vertices[vertexIdx]);
			tSubMesh->mIndexes.push_back(index);
		}
	}

	//free up
	if(layerElementUV)
		delete [] layerElementUV;

	if(UVReferenceMode)
		delete [] UVReferenceMode;

	if(UVMappingMode)
		delete [] UVMappingMode;

}

void FBXConverter::ConvertSkeletonData(KFbxNode* pNode)
{
	KFbxNodeAttribute* pNodeAtt = pNode->GetNodeAttribute();
	KFbxSkeleton* fbxSkeleton = dynamic_cast<KFbxSkeleton*>(pNodeAtt);

}

void FBXConverter::ConvertSkeleton(KFbxNode * pNode)
{
	
}

bool FBXConverter::ExportScene(const std::string& outfile)
{
	if(mConvertAscii)
	{
		std::string outfile_ascii = outfile + ".tam";

		fopen_s(&mpAsciiFile, outfile_ascii.c_str(), "w+");

		if(!mpAsciiFile)
		{
			printf("create file %s failed\n", outfile_ascii.c_str());
			return false;
		}
		fprintf_s(mpAsciiFile,"Titan Mesh File: %s \n", outfile_ascii.c_str());
	}

	// binary open
	std::string outfile_bin = outfile + ".tbm";
	fopen_s(&mpBinaryFile, outfile_bin.c_str(), "w+");

	if(!mpBinaryFile)
	{
		printf("create file %s failed\n", outfile_bin.c_str());
		return false;
	}

	/*
	*	Export every mesh
	*/
	for(int i = 0;i < mTiScene.mSubMeshes.size(); ++i)
	{
		ExportMesh(mTiScene.mSubMeshes.at(i));
	}

	/*
	*	skeleton data
	*/

	//close all file
	if(mConvertAscii)
		fclose(mpAsciiFile);

	fclose(mpBinaryFile);

	return true;
}

void FBXConverter::ExportMesh(TiSubMesh* mesh)
{
	//
	//binary export
	//
	WriteChunkHeaders(MCID_VertexFormat, mesh->mVertexDecls.size() * 3);
	WriteInts((const int*)&mesh->mVertexDecls[0].Semantic, mesh->mVertexDecls.size() * 3);

	//write vertex size
	int vertexElementSize = 0;
	for(auto it = mesh->mVertexData.begin(); it != mesh->mVertexData.end(); ++it)
	{
		int semanticIdx = FindVertexElement(*mesh, kVDS_POS, 0);
		if(semanticIdx != -1)
			vertexElementSize += 3;

		semanticIdx = FindVertexElement(*mesh,  kVDS_COLOR, 0);
		if(semanticIdx != -1)
			vertexElementSize += 1;

		for(int i = 0;i < 8;++i)
		{
			semanticIdx = FindVertexElement(*mesh, kVDS_TEXCOORD, i);
			if(semanticIdx != -1)
			{
				vertexElementSize += 2;
			}
			else
			{
				//we do not loop further
				break;
			}
		}

		semanticIdx = FindVertexElement(*mesh, kVDS_NORMAL, 0);
		if(semanticIdx != -1)
			vertexElementSize += 3;

	}
	//WriteChunkHeaders(MCID_SubMesh, )

	if(mpAsciiFile)
	{
		/*
		*	Vertex Data
		*/
		//here the %d is the vertex number
		fprintf_s(mpAsciiFile, "Vertex Data: num = %d, vertex format = ", mesh->mVertexData.size());
		//vertex decl
		for(int i = 0;i < mesh->mVertexDecls.size(); ++i)
		{
			fprintf_s(mpAsciiFile,"%d/%d/%d;", mesh->mVertexDecls[i].Semantic, mesh->mVertexDecls[i].SemanticIndex, mesh->mVertexDecls[i].ElementSizeInBytes);
		}
		fprintf_s(mpAsciiFile, "{ \n");
		//output vertex data
		for(auto it = mesh->mVertexData.begin(); it != mesh->mVertexData.end(); ++it)
		{
			int semanticIdx = FindVertexElement(*mesh, kVDS_POS, 0);
			if(semanticIdx != -1)
			{
				Titan::Vector3 pos = (*it).Position;
				fprintf_s(mpAsciiFile, "%f %f %f ", pos.x, pos.y, pos.z);
			}

			semanticIdx = FindVertexElement(*mesh,  kVDS_COLOR, 0);
			if(semanticIdx != -1)
			{
				fprintf_s(mpAsciiFile, "%d ", (*it).Color);
			}

			for(int i = 0;i < 8;++i)
			{
				semanticIdx = FindVertexElement(*mesh, kVDS_TEXCOORD, i);
				if(semanticIdx != -1)
				{
					float u = (*it).UVs.at(i).x, v = (*it).UVs.at(i).y;
					fprintf_s(mpAsciiFile, "%f %f ", u,v);
				}
				else
				{
					//we do not loop further
					break;
				}
			}

			semanticIdx = FindVertexElement(*mesh, kVDS_NORMAL, 0);
			if(semanticIdx != -1)
			{
				fprintf_s(mpAsciiFile, "%f %f %f", (*it).Normal.x, (*it).Normal.y, (*it).Normal.z);
			}
			fprintf_s(mpAsciiFile, "\n");
		}
		fprintf_s(mpAsciiFile, "};\n");

		/*
		*	Index Data
		*/
		fprintf_s(mpAsciiFile, "Index Data: %d {\n", mesh->mIndexes.size());
		for(int i = 0; i < mesh->mIndexes.size();i += 3)
		{
			fprintf_s(mpAsciiFile, "%d %d %d\n", mesh->mIndexes[i], mesh->mIndexes[i + 1], mesh->mIndexes[i + 2]);
		}
		fprintf_s(mpAsciiFile, "}\n");
	}
}

KFbxXMatrix FBXConverter::ComputeTotalMatrix(KFbxNode* Node)
{
	KFbxXMatrix Geometry;
	KFbxVector4 Translation, Rotation, Scaling;
	Translation = Node->GetGeometricTranslation(KFbxNode::eSOURCE_SET);
	Rotation = Node->GetGeometricRotation(KFbxNode::eSOURCE_SET);
	Scaling = Node->GetGeometricScaling(KFbxNode::eSOURCE_SET);
	Geometry.SetT(Translation);
	Geometry.SetR(Rotation);
	Geometry.SetS(Scaling);

	//For Single Matrix situation, obtain transfrom matrix from eDESTINATION_SET, which include pivot offsets and pre/post rotations.
	KFbxXMatrix& GlobalTransform = mpScene->GetEvaluator()->GetNodeGlobalTransform(Node);

	KFbxXMatrix TotalMatrix;
	TotalMatrix = GlobalTransform * Geometry;

	return TotalMatrix;
}

void FBXConverter::CleanSceneData()
{
	mTiScene.ClearScene();
}

void FBXConverter::WriteDatas(const void* pData, size_t size, size_t count)
{
	fwrite(pData, size, count, mpBinaryFile);
}

void FBXConverter::WriteFloats(const float* pData, size_t count)
{
	WriteDatas((void*)pData, sizeof(float), count);
}

void FBXConverter::WriteInts(const int* pData, size_t count)
{
	WriteDatas((void*)pData, sizeof(int), count);
}

void FBXConverter::WriteShorts(const short* pData, size_t count)
{
	WriteDatas((void*)pData, sizeof(short), count);
}

void FBXConverter::WriteBools(const bool* pData, size_t count)
{
	WriteDatas((void*)pData, sizeof(bool), count);
}

void FBXConverter::WriteChunkHeaders(short id, size_t size)
{
	WriteShorts(&id, 1);
	int Int = static_cast<int>(size);
	WriteInts(&Int, 1);
}


