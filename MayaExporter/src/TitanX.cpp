#include "MayaHeader.h"
#include "SceneTranslator.h"

//these headers must include in main cpp
#include <maya/MPlug.h>
#include <maya/MFnPlugin.h>
#include <maya/MPxFileTranslator.h>

class TitanXExporter : public MPxFileTranslator
{
public:
	TitanXExporter()
	{
		mpTranslator = new SceneTranslator();
	}

	virtual ~TitanXExporter()
	{
		delete mpTranslator;
	}

	static void*	creator();


	MStatus         reader ( const MFileObject& file,
		const MString& optionsString,
		FileAccessMode mode);

	MStatus         writer ( const MFileObject& file,
		const MString& optionsString,
		FileAccessMode mode );
	bool            haveReadMethod () const;
	bool            haveWriteMethod () const;
	MString         defaultExtension () const;
	MFileKind       identifyFile ( const MFileObject& fileName,
		const char* buffer,
		short size) const;

protected:
	MStatus         OutputPolygons( MDagPath&, MObject& );
	MStatus         exportSelected();
	MStatus         exportAll();
	
	void            setToLongUnitName( const MDistance::Unit&, MString& );
	

protected:
	SceneTranslator*	mpTranslator;
	// counters
	int v,vt,vn;
	// offsets
	int voff,vtoff,vnoff;
	// options
	bool groups, ptgroups, materials, smoothing, normals;

	FILE *mFilePtr;

	// Keeps track of all sets.
	//
	int numSets;
	MObjectArray *sets;

	// Keeps track of all objects and components.
	// The Tables are used to mark which sets each 
	// component belongs to.
	//
	MStringArray *objectNames;

	bool **polygonTablePtr;
	bool **vertexTablePtr;
	bool * polygonTable;
	bool * vertexTable;
	bool **objectGroupsTablePtr;


	// Used to determine if the last set(s) written out are the same
	// as the current sets to be written. We don't need to write out
	// sets unless they change between components. Same goes for
	// materials.
	//
	MIntArray *lastSets;
	MIntArray *lastMaterials;

	// We have to do 2 dag iterations so keep track of the
	// objects found in the first iteration by this index.
	//
	int objectId;
	int objectCount;


	int *           polySmoothingGroups;
	int             edgeTableSize;
	int             nextSmoothingGroup;
	int             currSmoothingGroup;
	bool            mNewSmoothingGroup;

	// List of names of the mesh shapes that we export from maya
	MStringArray	objectNodeNamesArray;

	// Used to keep track of Maya groups (transform DAG nodes) that
	// contain objects being exported
	MStringArray	transformNodeNameArray;

	bool	mExportPos;
	bool	mExportIndex;

};

char * objOptionScript = "objExportOptions";
char * objDefaultOptions =
"groups=1;"
"ptgroups=1;"
"materials=1;"
"smoothing=1;"
"normals=1;"
;

void* TitanXExporter::creator()
{
	return new TitanXExporter();
}

//////////////////////////////////////////////////////////////

MStatus TitanXExporter::reader ( const MFileObject& file,
								const MString& options,
								FileAccessMode mode)
{
	fprintf(stderr, "TitanXExporter::reader called in error\n");
	return MS::kFailure;
}


MStatus TitanXExporter::writer ( const MFileObject& file,
								const MString& options,
								FileAccessMode mode )

{
	MStatus status;

	mpTranslator->processMayaScene();

	MString mname = file.fullName(), unitName;

	const char *fname = mname.asChar();
	fopen_s(&mFilePtr, fname,"w");


	if (mFilePtr == NULL)
	{
		cerr << "Error: The file " << fname << " could not be opened for writing." << endl;
		return MS::kFailure;
	}

	// Options
	//
	groups      = true; // write out facet groups
	ptgroups    = true; // write out vertex groups
	materials   = true; // write out shading groups
	smoothing   = true; // write out facet smoothing information
	normals     = true; // write out normal table and facet normals


	mExportPos = true;
	mExportIndex = true;

	if (options.length() > 0) 
	{
		int i, length;
		// Start parsing.
		MStringArray optionList;
		MStringArray theOption;
		options.split(';', optionList); // break out all the options.

		length = optionList.length();
		for( i = 0; i < length; ++i )
		{
			theOption.clear();
			optionList[i].split( '=', theOption );
			if( theOption[0] == MString("groups") &&
				theOption.length() > 1 ) 
			{
					if( theOption[1].asInt() > 0 )
					{
						groups = true;
					}else{
						groups = false;
					}
			}
		}
	}

	/* print current linear units used as a comment in the obj file */
	setToLongUnitName(MDistance::uiUnit(), unitName);
	//fprintf( mFilePtr, "# This file uses %s as units for non-parametric coordinates.\n\n", unitName.asChar() ); 
	fprintf( mFilePtr, "# The units used in this file are %s.\n", unitName.asChar() );

	if( ( mode == MPxFileTranslator::kExportAccessMode ) ||
		( mode == MPxFileTranslator::kSaveAccessMode ) )
	{
		exportAll();
	}
	else if( mode == MPxFileTranslator::kExportActiveAccessMode )
	{
		exportSelected();
	}
	fclose(mFilePtr);

	return MS::kSuccess;
}
//////////////////////////////////////////////////////////////

void TitanXExporter::setToLongUnitName(const MDistance::Unit &unit, MString& unitName)
{
	switch( unit ) 
	{
	case MDistance::kInches:
		/// Inches
		unitName = "inches";
		break;
	case MDistance::kFeet:
		/// Feet
		unitName = "feet";
		break;
	case MDistance::kYards:
		/// Yards
		unitName = "yards";
		break;
	case MDistance::kMiles:
		/// Miles
		unitName = "miles";
		break;
	case MDistance::kMillimeters:
		/// Millimeters
		unitName = "millimeters";
		break;
	case MDistance::kCentimeters:
		/// Centimeters
		unitName = "centimeters";
		break;
	case MDistance::kKilometers:
		/// Kilometers
		unitName = "kilometers";
		break;
	case MDistance::kMeters:
		/// Meters
		unitName = "meters";
		break;
	default:
		break;
	}
}
//////////////////////////////////////////////////////////////

bool TitanXExporter::haveReadMethod () const
{
	return false;
}
//////////////////////////////////////////////////////////////

bool TitanXExporter::haveWriteMethod () const
{
	return true;
}
//////////////////////////////////////////////////////////////

MString TitanXExporter::defaultExtension () const
{
	return "xmesh";
}
//////////////////////////////////////////////////////////////

MPxFileTranslator::MFileKind TitanXExporter::identifyFile (
	const MFileObject& fileName,
	const char* buffer,
	short size) const
{
	const char * name = fileName.name().asChar();
	int   nameLength = strlen(name);

	if ((nameLength > 5) && !_stricmp(name+nameLength - 5, ".xmesh"))
		return kCouldBeMyFileType;
	else
		return kNotMyFileType;
}
//////////////////////////////////////////////////////////////

MStatus initializePlugin( MObject obj )
{
	MFnPlugin plugin( obj, "Blz", "3.0", "Any");

	// Register the translator with the system
	return plugin.registerFileTranslator( "TitanX", "none",
		TitanXExporter::creator,
		(char *)objOptionScript,
		(char *)objDefaultOptions );                                        
}
//////////////////////////////////////////////////////////////

MStatus uninitializePlugin( MObject obj )
{
	MFnPlugin plugin( obj );
	return plugin.deregisterFileTranslator( "TitanX" );
}

//////////////////////////////////////////////////////////////

MStatus TitanXExporter::OutputPolygons(MDagPath& mdagPath, MObject&  mComponent)
{
	MStatus stat = MS::kSuccess;
	MSpace::Space space = MSpace::kWorld;

	MFnMesh fnMesh( mdagPath, &stat );
	if ( MS::kSuccess != stat) 
	{
		fprintf(stderr,"Failure in MFnMesh initialization.\n");
		return MS::kFailure;
	}

	MItMeshPolygon polyIter( mdagPath, mComponent, &stat );
	if ( MS::kSuccess != stat) 
	{
		fprintf(stderr,"Failure in MItMeshPolygon initialization.\n");
		return MS::kFailure;
	}
	MItMeshVertex vtxIter( mdagPath, mComponent, &stat );
	if ( MS::kSuccess != stat) 
	{
		fprintf(stderr,"Failure in MItMeshVertex initialization.\n");
		return MS::kFailure;
	}

	// Write out the vertex table
	//
	fprintf(mFilePtr, "pos\n");

	fprintf(mFilePtr, "vertex data{\n");

	for ( ; !vtxIter.isDone(); vtxIter.next() )
	{
		if(mExportPos)
		{
			MPoint p = vtxIter.position( space );

			// convert from internal units to the current ui units
			p.x = MDistance::internalToUI(p.x);
			p.y = MDistance::internalToUI(p.y);
			p.z = MDistance::internalToUI(p.z);
			fprintf(mFilePtr,"%f %f %f ",p.x,p.y,p.z);
			fprintf(mFilePtr, "\n");
		}
		
		v++;
	}

	fprintf(mFilePtr, "}\n");

	// Write out the uv table
	//
	MFloatArray uArray, vArray;
	fnMesh.getUVs( uArray, vArray );
	int uvLength = uArray.length();
	for ( int x=0; x<uvLength; x++ ) 
	{
		fprintf(mFilePtr,"vt %f %f\n",uArray[x],vArray[x]);
		vt++;
	}

	// Write out the normal table
	
	return stat;
}

//////////////////////////////////////////////////////////////

MStatus TitanXExporter::exportSelected( )
{
	MStatus status;
	MString filename;


	// Create an iterator for the active selection list
	//
	MSelectionList slist;
	MGlobal::getActiveSelectionList( slist );
	MItSelectionList iter( slist );

	if (iter.isDone()) 
	{
		fprintf(stderr,"Error: Nothing is selected.\n");
		return MS::kFailure;
	}

	// We will need to interate over a selected node's heirarchy 
	// in the case where shapes are grouped, and the group is selected.
	MItDag dagIterator( MItDag::kDepthFirst, MFn::kInvalid, &status);

	// reset counters
	v = vt = vn = 0;
	voff = vtoff = vnoff = 0;

	// Selection list loop
	for ( ; !iter.isDone(); iter.next() )
	{	 
		MDagPath objectPath;
		// get the selected node
		status = iter.getDagPath( objectPath);

		// reset iterator's root node to be the selected node.
		status = dagIterator.reset (objectPath.node(), 
			MItDag::kDepthFirst, MFn::kInvalid );	

		// DAG iteration beginning at at selected node
		for ( ; !dagIterator.isDone(); dagIterator.next() )
		{
			MDagPath dagPath;
			MObject  component = MObject::kNullObj;
			status = dagIterator.getPath(dagPath);

			if (!status) {
				fprintf(stderr,"Failure getting DAG path.\n");
				return MS::kFailure;
			}

			if (status ) 
			{
				// skip over intermediate objects
				//
				MFnDagNode dagNode( dagPath, &status );
				if (dagNode.isIntermediateObject()) 
				{
					continue;
				}

				if (dagPath.hasFn(MFn::kNurbsSurface))
				{
					status = MS::kSuccess;
					fprintf(stderr,"Warning: skipping Nurbs Surface.\n");
				}
				else if ((  dagPath.hasFn(MFn::kMesh)) &&
					(  dagPath.hasFn(MFn::kTransform)))
				{
					// We want only the shape, 
					// not the transform-extended-to-shape.
					continue;
				}
				else if (  dagPath.hasFn(MFn::kMesh))
				{

					status = OutputPolygons(dagPath, component);
					objectId++;
					if (status != MS::kSuccess) {
						fprintf(stderr, "Error: exporting geom failed, check your selection.\n");
			
						return MS::kFailure;
					}		
				}
				voff = v;
				vtoff = vt;
				vnoff = vn;
			}
		}
	}

	return status;
}

//////////////////////////////////////////////////////////////

MStatus TitanXExporter::exportAll( )
{
	MStatus status = MS::kSuccess;

	MItDag dagIterator( MItDag::kBreadthFirst, MFn::kInvalid, &status);

	if ( MS::kSuccess != status) 
	{
		fprintf(stderr,"Failure in DAG iterator setup.\n");
		return MS::kFailure;
	}
	// reset counters
	v = vt = vn = 0;
	voff = vtoff = vnoff = 0;

	for ( ; !dagIterator.isDone(); dagIterator.next() )
	{
		MDagPath dagPath;
		MObject  component = MObject::kNullObj;
		status = dagIterator.getPath(dagPath);

		if (!status) 
		{
			fprintf(stderr,"Failure getting DAG path.\n");
			return MS::kFailure;
		}

		// skip over intermediate objects
		//
		MFnDagNode dagNode( dagPath, &status );
		if (dagNode.isIntermediateObject()) 
		{
			continue;
		}

		if ((  dagPath.hasFn(MFn::kNurbsSurface)) &&
			(  dagPath.hasFn(MFn::kTransform)))
		{
			status = MS::kSuccess;
			fprintf(stderr,"Warning: skipping Nurbs Surface.\n");
		}
		else if ((  dagPath.hasFn(MFn::kMesh)) &&
			(  dagPath.hasFn(MFn::kTransform)))
		{
			// We want only the shape, 
			// not the transform-extended-to-shape.
			continue;
		}
		else if (  dagPath.hasFn(MFn::kMesh))
		{
			// Now output the polygon information
			//
			status = OutputPolygons(dagPath, component);
			objectId++;
			if (status != MS::kSuccess) 
			{
				fprintf(stderr,"Error: exporting geom failed.\n");		
				return MS::kFailure;
			}
		}
		voff = v;
		vtoff = vt;
		vnoff = vn;
	}

	return status;
}
