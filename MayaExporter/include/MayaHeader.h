#ifndef TITAN_MAYA_HEADER__HH
#define TITAN_MAYA_HEADER__HH


#include <iostream>
#include <string.h> 

#include <maya/MFnPlugin.h>
#include <maya/MPlug.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MFnDagNode.h>
#include <maya/MItDag.h>
#include <maya/MDistance.h>
#include <maya/MIOStream.h>
#include <maya/MFileIO.h>
#include <maya/MStatus.h>
#include <maya/MArgList.h>
#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnDagNode.h>
#include <maya/MMatrix.h>
#include <maya/MGlobal.h>

#include <maya/MFnMesh.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MIntArray.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MPointArray.h>
#include <maya/MDoubleARray.h>
#include <maya/MObjectArray.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MItGeometry.h>
#include <maya/MFnBlendShapeDeformer.h>

#include <maya/MPxNode.h>
#include <maya/MPxCommand.h>
#include <maya/MPxData.h>

#include <maya/MFnSkinCluster.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItMeshEdge.h>
#include <maya/MFnTransform.h>
#include <maya/MEulerRotation.h>
#include <maya/MAnimControl.h>

#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>

#endif