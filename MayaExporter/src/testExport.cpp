#include <iostream>
#include <string.h> 
#include <sys/types.h>
#include <maya/MStatus.h>
#include <maya/MPxCommand.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnPlugin.h>
#include <maya/MFnMesh.h>
#include <maya/MFnSet.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItMeshEdge.h>
#include <maya/MFloatVector.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatArray.h>
#include <maya/MObjectArray.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MFnDagNode.h>
#include <maya/MItDag.h>
#include <maya/MDistance.h>
#include <maya/MIntArray.h>
#include <maya/MIOStream.h>

#define TITAN_EXPORT_UV_NUM 8
//add later....

#define SAFE_DELETE(p) if(p){ delete p; p = NULL;}
#define SAFE_FREE(p) if(p){free(p); p = NULL;}

char *	cScriptName = "testExport";
char *	cDefaultParams = "\
						position=1\
";

class TitanExporter : public MPxFileTranslator
{
public:
	TitanExporter(){};

	virtual ~TitanExporter(){};

	static void*    creator();

	virtual MStatus		reader ( const MFileObject& file,
		const MString& optionsString,
		FileAccessMode mode);
	virtual MStatus		writer ( const MFileObject& file,
		const MString& optionsString,
		FileAccessMode mode);

	virtual bool		haveReadMethod () const;
	virtual bool		haveWriteMethod () const;

	virtual MString     defaultExtension () const;


	virtual MPxFileTranslator::MFileKind identifyFile (	const MFileObject& file,
		const char* buffer, short size) const;

	static MPxFileTranslator::FileAccessMode fileAccessMode();

protected:

	MStatus exportAll();

	MStatus exportSelected();

	MStatus	outputMeshData(MDagPath& dagPath, MObject& obj);


protected:
	FILE*	mFilePtr;

	int				mObjId;
	int				mObjCount;

	int				mCurVtxAccum, mPreVtxAccum;

	bool			mExportPos;
	bool			mExportUV[TITAN_EXPORT_UV_NUM];
	bool			mExportColor;
	bool			mExportNormal;

	bool			mExportBinary;

	int				mPrimitiveType;// change later
};

MStatus initializePlugin( MObject obj)
{
	MFnPlugin plugin(obj, "blz", "3.0", "Any");

	//lack of mel script and default params
	return plugin.registerFileTranslator("TitanX", "none",
		TitanExporter::creator,
		cScriptName,
		cDefaultParams);
}

MStatus uninitializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin	plugin(obj);
	status = plugin.deregisterFileTranslator("TitanX");
	if(!status)
	{
		status.perror("uninitializePlugin");
	}
	return status;
}

void*   TitanExporter::creator()
{
	return new TitanExporter();
}

MStatus	TitanExporter::reader(const MFileObject& file, const MString& optionsString, FileAccessMode mode)
{
	fprintf(stderr, "temporialy does not support reading\n");
	return MStatus::kNotImplemented;
}

bool TitanExporter::haveReadMethod() const
{
	return false;
}

MString TitanExporter::defaultExtension() const
{
	return "xmesh";
}

MPxFileTranslator::MFileKind TitanExporter::identifyFile(const MFileObject& file, const char* buffer, short size) const
{
	const char * name = file.name().asChar();
	int   nameLength = strlen(name);

	if ((nameLength > 5) && !_stricmp(name + nameLength - 5, ".xmesh"))
		return kCouldBeMyFileType;
	else
		return kNotMyFileType;
}

bool TitanExporter::haveWriteMethod() const 
{
	return true;
}

MStatus TitanExporter::writer(const MFileObject& file, const MString& optionsString, FileAccessMode mode)
{
	MStatus status = MStatus::kSuccess;

	MString filename = file.name();

	fopen_s(&mFilePtr, filename.asChar(), "wb");

	if (mFilePtr == NULL)
	{
		fprintf(stderr, "open file %s failed\n", filename.asChar());
		return MStatus::kFailure;
	}

	//default export data
	mExportPos = true;
	mExportColor = false;
	mExportNormal = true;
	for(int i = 0; i < TITAN_EXPORT_UV_NUM; ++i)
		mExportUV[i] = false;
 
	//for test
	mExportUV[0] = true;

	mPrimitiveType = 0; //triangle list

	
	if (optionsString.length() > 0)
	{
		MStringArray optionList;
		optionsString.split(';', optionList);
		for (unsigned int i = 0; i < optionList.length(); ++i)
		{
			MStringArray options;
			optionList[i].split('=', options);
			if (options[0] == "position" && options[1].length() > 0)
			{
				mExportPos = ((options[1].asInt() == 1) ? true : false);
			}
		}
	}
	
	if(mode == MPxFileTranslator::kExportAccessMode ||
		mode == MPxFileTranslator::kSaveAccessMode)
	{
		status = exportAll();
	}
	else if(mode == MPxFileTranslator::kExportActiveAccessMode)
	{
		status = exportSelected();
	}
	
	fclose(mFilePtr);
	return status;
}

MStatus TitanExporter::exportAll()
{
	MStatus status = MStatus::kSuccess;


	MItDag dagIt(MItDag::kBreadthFirst, MFn::kInvalid, &status);

	if(status != MS::kSuccess)
	{
		fprintf(stderr, "failed to create MItDag\n");
		return status;
	}

	mCurVtxAccum = mPreVtxAccum = 0;

	for (; !dagIt.isDone(); dagIt.next())
	{
		MDagPath path;
		MObject obj;

		status = dagIt.getPath(path);
		if(status != MS::kSuccess)
		{
			return MStatus::kFailure;
		}

		MFnDagNode dagNode(path, &status);
		if (dagNode.isIntermediateObject())
		{
			continue;
		}
		
		if (path.hasFn(MFn::kNurbsSurface) &&
			path.hasFn(MFn::kTransform))
		{
			continue;
		}
		else if(path.hasFn(MFn::kMesh)&&
			path.hasFn(MFn::kTransform))
		{
			continue;
		}
		else if (path.hasFn(MFn::kMesh))
		{
			status = outputMeshData(path, obj);

			if(status != MS::kSuccess)
			{
				fprintf(stderr, "export mesh failed.\n");
				return status;
			}
		}
		mPreVtxAccum = mCurVtxAccum;
	}
	
	return status;
}

MStatus TitanExporter::exportSelected()
{
	MStatus status;

	return status;
}


MStatus TitanExporter::outputMeshData(MDagPath& dagPath, MObject& obj)
{
	MStatus stat = MStatus::kSuccess;
	MSpace::Space space = MSpace::kWorld;

	MFnMesh fnMesh(dagPath, &stat);

	if(stat != MS::kSuccess)
	{
		fprintf(stderr, "failed in create MFnMesh\n");
		return stat;
	}

	fprintf(mFilePtr, "mesh %s {\n", fnMesh.name().asChar());
	
	MItMeshPolygon pit(dagPath, obj, &stat);
	if(stat != MS::kSuccess)
	{
		fprintf(stderr, "MItMeshPolygon error\n");
		return stat;
	}

	MItMeshVertex vit(dagPath, obj, &stat);
	if(stat != MS::kSuccess)
	{
		fprintf(stderr, "MItMeshVertex error\n");
		return stat;
	}

	fprintf(mFilePtr, "export data: ");
	if(mExportPos)
		fprintf(mFilePtr, "pos ");
	
	for(unsigned int i = 0;i < TITAN_EXPORT_UV_NUM; ++i)
	{	
		if(mExportUV[i])
			fprintf(mFilePtr, "uv%d ", i);
	}
	if(mExportNormal)
		fprintf(mFilePtr, "normal ");
	if(mExportColor)
		fprintf(mFilePtr, "color ");

	fprintf(mFilePtr, ";\n");
	//we need add sub mesh group

	//vertex data output
	fprintf(mFilePtr, "Vertex Data:%d {\n", fnMesh.numVertices());
	for ( ; !vit.isDone(); vit.next() ) 
	{
		if(mExportPos)
		{
			MPoint p = vit.position( space );
			// convert from internal units to the current ui units
			p.x = MDistance::internalToUI(p.x);
			p.y = MDistance::internalToUI(p.y);
			p.z = MDistance::internalToUI(p.z);
			fprintf(mFilePtr,"%f %f %f ",p.x,p.y,p.z);
		}
		for(int i = 0;i < TITAN_EXPORT_UV_NUM; ++i)
		{
			if(mExportUV[i])
			{
				float2 uvSets;
				vit.getUV(uvSets);
				fprintf(mFilePtr, "%f %f ", uvSets[0], uvSets[1]);
			}	
		}
		if (mExportNormal)
		{
			MVector normal;
			vit.getNormal(normal);
			fprintf(mFilePtr, "%f %f %f ", normal.x, normal.y, normal.z);
		}
		if (mExportColor)
		{
			MColor vertexColor;
			vit.getColor(vertexColor);
			fprintf(mFilePtr, "%f %f %f %f ", vertexColor.a, vertexColor.r, vertexColor.g, vertexColor.b);
		}
		
		fprintf(mFilePtr, ";\n");
		++mCurVtxAccum;
	}

	fprintf(mFilePtr, "}//vertex data\n");

	//index data
	fprintf(mFilePtr, "index type: %d\n", mPrimitiveType);
	fprintf(mFilePtr, "index data{\n");
	for( ; !pit.isDone(); pit.next())
	{
		int triCount;
		pit.numTriangles(triCount);
		if(mPrimitiveType == 0)// triangle list
		{
			for(int i = 0; i < triCount; ++i)
			{
				MPointArray pos;
				MIntArray indices;
				pit.getTriangle(i, pos, indices);
				if(indices.length() != 3)
				{
					fprintf(stderr, "triangle list indices is 3!");
					continue;
				}
				fprintf(mFilePtr, "%d %d %d\n", 
					indices[0] + 1 + mPreVtxAccum,
					indices[1] + 1 + mPreVtxAccum,
					indices[2] + 1 + mPreVtxAccum);
			}
		}
		else
		{
			//add other primitive type later
		}
		
	}

	fprintf(mFilePtr, "}//index data\n");
	fprintf(mFilePtr, "}//mesh\n");
	fflush(mFilePtr);

	MItDag dagIt(MItDag::kBreadthFirst, MFn::kFileTexture, &stat);
	for(; !dagIt.isDone(); dagIt.next())
	{
		MObject texObj;
		MFnDependencyNode dgNodeFn;
		
	}

	return stat;
}
