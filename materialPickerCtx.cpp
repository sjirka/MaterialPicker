#include "MaterialPickerCtx.h"

#include <maya\MGlobal.h>
#include <maya\MFnDependencyNode.h>
#include <maya\MSelectionList.h>
#include <maya\MItSelectionList.h>
#include <maya\MFnMesh.h>
#include <maya\MFnNurbsSurface.h>
#include <maya\MFnSubd.h>
#include <maya\MFnNurbsCurve.h>
#include <maya\M3dView.h>
#include <maya\MFnMeshData.h>
#include <maya\MFnCamera.h>
#include <maya\MMatrix.h>
#include <maya\MFnNurbsCurveData.h>
#include <maya\MFnTransform.h>
#include <maya\MBoundingBox.h>
#include <maya\MCursor.h>

#define pickCursor_width 32
#define pickCursor_height 32
static unsigned char pickCursor_bits[] = {
	0xFF, 0xFF, 0x0F, 0x00, 0xFF, 0xFF, 0x7F, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
	0xFF, 0xFF, 0xFF, 0x03, 0xFF, 0x1F, 0xF8, 0x07, 0xFF, 0x03, 0xC0, 0x0F,
	0xFF, 0x01, 0x80, 0x1F, 0x7F, 0x00, 0x00, 0x1F, 0x7F, 0x00, 0x00, 0x3E,
	0x3F, 0x00, 0x00, 0x7C, 0x1F, 0x00, 0x00, 0x7C, 0x1F, 0x00, 0x00, 0x78,
	0x1F, 0x00, 0x00, 0x78, 0x0F, 0x00, 0x00, 0xF8, 0x0F, 0x00, 0x00, 0xF0,
	0x0F, 0x00, 0x00, 0xF0, 0x0F, 0x00, 0x00, 0xF0, 0x0F, 0x00, 0x00, 0xF0,
	0x0F, 0x00, 0x00, 0xF8, 0x1F, 0x00, 0x00, 0x78, 0x1E, 0x00, 0x00, 0x78,
	0x1E, 0x00, 0x00, 0x7C, 0x3E, 0x00, 0x00, 0x7C, 0x7C, 0x00, 0x00, 0x3E,
	0x78, 0x00, 0x00, 0x1F, 0xF8, 0x01, 0x80, 0x1F, 0xF0, 0x03, 0xE0, 0x0F,
	0xE0, 0x3F, 0xFC, 0x07, 0xC0, 0xFF, 0xFF, 0x03, 0x00, 0xFF, 0xFF, 0x00,
	0x00, 0xFE, 0x7F, 0x00, 0x00, 0xE0, 0x07, 0x00, };


MaterialPickerCtx::MaterialPickerCtx(){
	setTitleString("Material Picker");
	setImage("eyeDropper.xpm", MPxContext::kImage1);
}

MaterialPickerCtx::~MaterialPickerCtx()
{
}

void MaterialPickerCtx::toolOnSetup(MEvent &event) {
	setHelpString("Pick shader from geometry with left mouse button. Assign stored shader with middle mouse button");
	
	MCursor pickCursor(pickCursor_width, pickCursor_height, 1, 1, pickCursor_bits, pickCursor_bits);
	setCursor(pickCursor);
}

void MaterialPickerCtx::doEnterRegion() {
	setHelpString("Pick shader from geometry with left mouse button. Assign stored shader with middle mouse button");
}

void MaterialPickerCtx::toolOffCleanup() {
	MPxContext::toolOffCleanup();
}

MString MaterialPickerCtx::stringClassName() const {
	return "materialPickerCtx";
}

// VP2
MStatus MaterialPickerCtx::doPress(MEvent &event, MHWRender::MUIDrawManager &drawMgr, const MHWRender::MFrameContext &context) {
	return doPress(event);
}

MStatus MaterialPickerCtx::doDrag(MEvent &event, MHWRender::MUIDrawManager &drawMgr, const MHWRender::MFrameContext &context) {
	return doDrag(event);
}

MStatus MaterialPickerCtx::doRelease(MEvent &event, MHWRender::MUIDrawManager &drawMgr, const MHWRender::MFrameContext &context) {
	return doRelease(event);
}

// Common
MStatus MaterialPickerCtx::doPress(MEvent &event){
	MStatus status;

	return MS::kSuccess;
}

MStatus MaterialPickerCtx::doDrag(MEvent &event) {
	MStatus status;

	return MS::kSuccess;
}

MStatus MaterialPickerCtx::doRelease(MEvent &event){
	MStatus status;

	MSelectionList originalSelection;
	status = MGlobal::getActiveSelectionList(originalSelection, true);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	// Click select geometry
	short x, y;
	event.getPosition(x, y);
	status = MGlobal::selectFromScreen(x, y, MGlobal::kReplaceList, MGlobal::selectionMethod());
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MSelectionList selection;
	status = MGlobal::getActiveSelectionList(selection);
	CHECK_MSTATUS_AND_RETURN_IT(status)

	if (selection.length() == 0)
		return MGlobal::selectCommand(originalSelection, MGlobal::kReplaceList);

	MDagPath path;
	status = selection.getDagPath(0, path);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	status = path.extendToShapeDirectlyBelow(0);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (event.mouseButton()==MEvent::kLeftMouse) {
		// Get the view based ray
		MPoint source, pivot;
		MVector ray;
		bool hit;
		M3dView currentView = M3dView::active3dView(&status);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		status = currentView.viewToWorld(x, y, source, ray);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Find ray/geometry intersection
		if (path.apiType() == MFn::kMesh) {
			MFnMesh fnMesh(path, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);
			MFloatPoint intersectionFloat;
			MMeshIsectAccelParams accelParams = fnMesh.autoUniformGridParams();
			int hitFace, hitTriangle;
			float hitBary1, hitBary2, hitRayParam;
			hit = fnMesh.closestIntersection(source, ray, NULL, NULL, false, MSpace::kWorld, 99999999.9f, false, &accelParams, intersectionFloat, &hitRayParam, &hitFace, &hitTriangle, &hitBary1, &hitBary2, 0.01f, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			MObjectArray shaders;
			MIntArray shIndices;
			fnMesh.getConnectedShaders(path.instanceNumber(), shaders, shIndices);

			m_shader = (shIndices[hitFace] >= 0) ? shaders[shIndices[hitFace]] : MObject::kNullObj;
		}
		else if (path.apiType() == MFn::kNurbsSurface) {
			MFnNurbsSurface fnNurbs(path, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Get nurbs intersections and use the closest
			double u, v;
			hit = fnNurbs.intersect(source, ray, u, v, pivot, 0.01, MSpace::kWorld);

			MDoubleArray knotsU, knotsV;
			fnNurbs.getKnotsInU(knotsU);
			fnNurbs.getKnotsInV(knotsV);

			int patchId[2] = { 0, 0 };
			for (unsigned int i = fnNurbs.degreeU() - 1; i < knotsU.length() - fnNurbs.degreeU(); i++)
				if (u > knotsU[i] && u < knotsU[i + 1]) {
					patchId[0] = i - fnNurbs.degreeU() + 1;
					break;
				}
			for (unsigned int i = fnNurbs.degreeV() - 1; i < knotsV.length() - fnNurbs.degreeV(); i++)
				if (v > knotsV[i] && v < knotsV[i + 1]) {
					patchId[1] = i - fnNurbs.degreeV() + 1;
					break;
				}

			MObjectArray shaders;
			MIntArray shIndices;
			fnNurbs.getConnectedShaders(path.instanceNumber(), shaders, shIndices);

			int patchIdx = patchId[0] + patchId[1] * fnNurbs.numSpansInU();
			m_shader = (shIndices[patchIdx] >= 0) ? shaders[shIndices[patchIdx]] : MObject::kNullObj;
		}
		/*
		else if (path.apiType() == MFn::kSubdiv) {
			// First convert SubD to proxy Mesh
			MFnSubd fnSubd(path, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);
			MFnMeshData dataCreator;
			MObject tesselated = dataCreator.create(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);
			tesselated = fnSubd.tesselate(true, 1, 1, tesselated, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);
			MMatrix matrix = path.inclusiveMatrix();

			// Apply world transformations to proxy
			MFnMesh fnMesh(tesselated, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);
			MPointArray positions;
			status = fnMesh.getPoints(positions);
			CHECK_MSTATUS_AND_RETURN_IT(status);
			for (unsigned int i = 0; i < positions.length(); i++)
				positions[i] *= matrix;
			fnMesh.setPoints(positions);

			// Get closest hit on the proxy mesh
			hit = meshClosestIntersection(tesselated, source, ray, pivot, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);
		}
		*/
		if (!m_shader.isNull()) {
			MFnDependencyNode fnShader(m_shader);
			MGlobal::displayInfo("store shader:"+fnShader.name());
		}
	}
	else if(event.mouseButton() == MEvent::kMiddleMouse){
		if (!m_shader.isNull()) {
			MFnDependencyNode fnShader(m_shader);
			MGlobal::executeCommand("hyperShade -assign " + fnShader.name(), true, true);
		}
	}

	MGlobal::selectCommand(originalSelection, MGlobal::kReplaceList);

	return MS::kSuccess;
}

MPoint MaterialPickerCtx::closestPoint(MPointArray &cloud, MPoint &toPoint) {
	MPoint closestPoint;
	double closestDistance;
	for (unsigned d = 0; d < cloud.length(); d++) {
		double distance = (cloud[d] - toPoint).length();
		if (d == 0 || distance < closestDistance) {
			closestDistance = distance;
			closestPoint = cloud[d];
		}
	}
	return closestPoint;
}