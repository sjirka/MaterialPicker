#include "MaterialPickerCtx.h"
#include "../_library/SCamera.h"

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
#include <maya\MRichSelection.h>

#define materialPicker_width 32
#define materialPicker_height 32
#define materialPicker_x_hot 1
#define materialPicker_y_hot 32
static unsigned char materialPicker_bits[] = {
	0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x80, 0x7F,
	0x00, 0x00, 0xC0, 0xFF, 0x00, 0x00, 0xE3, 0xFF, 0x00, 0x80, 0xF7, 0xFF,
	0x00, 0xC0, 0xFF, 0xFF, 0x00, 0xC0, 0xFF, 0x7F, 0x00, 0xC0, 0xFF, 0x3F,
	0x00, 0x80, 0xFF, 0x1F, 0x00, 0xC0, 0xFF, 0x0F, 0x00, 0xE0, 0xFE, 0x07,
	0x00, 0x70, 0xFC, 0x03, 0x00, 0x38, 0xF8, 0x07, 0x00, 0x1C, 0xF0, 0x0F,
	0x00, 0x0E, 0xE0, 0x0F, 0x00, 0x07, 0xF0, 0x07, 0x80, 0x03, 0xB8, 0x03,
	0xC0, 0x01, 0x1C, 0x00, 0xE0, 0x7C, 0x0E, 0x00, 0x70, 0x3E, 0x07, 0x00,
	0x38, 0x9F, 0x03, 0x00, 0x9C, 0xCF, 0x01, 0x00, 0xCC, 0xE7, 0x00, 0x00,
	0xEC, 0x73, 0x00, 0x00, 0xEC, 0x39, 0x00, 0x00, 0xEE, 0x1C, 0x00, 0x00,
	0x07, 0x0E, 0x00, 0x00, 0xE3, 0x07, 0x00, 0x00, 0xF3, 0x03, 0x00, 0x00,
	0x3F, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, };

static unsigned char materialPicker_empty_bits[] = {
	0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x80, 0x7F,
	0x00, 0x00, 0xC0, 0xFF, 0x00, 0x00, 0xE3, 0xFF, 0x00, 0x80, 0xF7, 0xFF,
	0x00, 0xC0, 0xFF, 0xFF, 0x00, 0xC0, 0xFF, 0x7F, 0x00, 0xC0, 0xFF, 0x3F,
	0x00, 0x80, 0xFF, 0x1F, 0x00, 0xC0, 0xFF, 0x0F, 0x00, 0xE0, 0xFE, 0x07,
	0x00, 0x70, 0xFC, 0x03, 0x00, 0x38, 0xF8, 0x07, 0x00, 0x1C, 0xF0, 0x0F,
	0x00, 0x0E, 0xE0, 0x0F, 0x00, 0x07, 0xF0, 0x07, 0x80, 0x03, 0xB8, 0x03,
	0xC0, 0x01, 0x1C, 0x00, 0xE0, 0x00, 0x0E, 0x00, 0x70, 0x00, 0x07, 0x00,
	0x38, 0x80, 0x03, 0x00, 0x1C, 0xC0, 0x01, 0x00, 0x0C, 0xE0, 0x00, 0x00,
	0x0C, 0x70, 0x00, 0x00, 0x0C, 0x38, 0x00, 0x00, 0x0E, 0x1C, 0x00, 0x00,
	0x07, 0x0E, 0x00, 0x00, 0xE3, 0x07, 0x00, 0x00, 0xF3, 0x03, 0x00, 0x00,
	0x3F, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, };

static unsigned char materialPicker_full_bits[] = {
	0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x80, 0x7F,
	0x00, 0x00, 0xC0, 0xFF, 0x00, 0x00, 0xE3, 0xFF, 0x00, 0x80, 0xF7, 0xFF,
	0x00, 0xC0, 0xFF, 0xFF, 0x00, 0xC0, 0xFF, 0x7F, 0x00, 0xC0, 0xFF, 0x3F,
	0x00, 0x80, 0xFF, 0x1F, 0x00, 0xC0, 0xFF, 0x0F, 0x00, 0xE0, 0xFE, 0x07,
	0x00, 0x70, 0xFC, 0x03, 0x00, 0x38, 0xF9, 0x07, 0x00, 0x9C, 0xF3, 0x0F,
	0x00, 0xCE, 0xE7, 0x0F, 0x00, 0xE7, 0xF3, 0x07, 0x80, 0xF3, 0xB9, 0x03,
	0xC0, 0xF9, 0x1C, 0x00, 0xE0, 0x7C, 0x0E, 0x00, 0x70, 0x3E, 0x07, 0x00,
	0x38, 0x9F, 0x03, 0x00, 0x9C, 0xCF, 0x01, 0x00, 0xCC, 0xE7, 0x00, 0x00,
	0xEC, 0x73, 0x00, 0x00, 0xEC, 0x39, 0x00, 0x00, 0xEE, 0x1C, 0x00, 0x00,
	0x07, 0x0E, 0x00, 0x00, 0xE3, 0x07, 0x00, 0x00, 0xF3, 0x03, 0x00, 0x00,
	0x3F, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, };


MaterialPickerCtx::MaterialPickerCtx(){
	setTitleString("Material Picker");
	setImage("materialPicker.xpm", MPxContext::kImage1);

	MCursor materialPickerCursor(materialPicker_width, materialPicker_height, materialPicker_x_hot, materialPicker_y_hot, materialPicker_bits, materialPicker_bits);
	setCursor(materialPickerCursor);

	MSelectionList initialShadingGroups;
	MGlobal::getSelectionListByName("initialShadingGroup", initialShadingGroups);
	if (!initialShadingGroups.isEmpty())
		initialShadingGroups.getDependNode(0, m_shader);
}

MaterialPickerCtx::~MaterialPickerCtx()
{
}

void MaterialPickerCtx::toolOnSetup(MEvent &event) {
	m_helpString = "Pick new shader with left mouse button. Assign it with middle mouse button";
	setHelpString(m_helpString);
}

void MaterialPickerCtx::doEnterRegion() {
	setHelpString(m_helpString);
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

	MCursor materialPickerCursor(
		materialPicker_width,
		materialPicker_height,
		materialPicker_x_hot,
		materialPicker_y_hot,
		(event.mouseButton() == MEvent::kMiddleMouse) ? materialPicker_empty_bits : materialPicker_full_bits,
		(event.mouseButton() == MEvent::kMiddleMouse) ? materialPicker_empty_bits : materialPicker_full_bits);
	setCursor(materialPickerCursor);

	status = m_selectionState.storeCurrentSelection();
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;
}

MStatus MaterialPickerCtx::doDrag(MEvent &event) {
	MStatus status;

	return MS::kSuccess;
}

MStatus MaterialPickerCtx::doRelease(MEvent &event){
	MStatus status;

	MCursor materialPickerCursor(materialPicker_width, materialPicker_height, materialPicker_x_hot, materialPicker_y_hot, materialPicker_bits, materialPicker_bits);
	setCursor(materialPickerCursor);

	status = MGlobal::setHiliteList(MSelectionList());
	CHECK_MSTATUS_AND_RETURN_IT(status);
	
	// Click select geometry
	short x, y;
	event.getPosition(x, y);
	MGlobal::selectFromScreen(x, y, MGlobal::kReplaceList, MGlobal::kSurfaceSelectMethod);

	MSelectionList selection;
	MGlobal::getActiveSelectionList(selection);

	if (!selection.isEmpty()) {
		MDagPath path;
		status = selection.getDagPath(0, path);
		CHECK_MSTATUS_AND_RETURN_IT(status)
		status = path.extendToShape();
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (event.mouseButton() == MEvent::kLeftMouse) {
			// Get the view based ray
			MPoint source, pivot;
			MVector ray;
			MObjectArray shaders;
			MIntArray shIndices;
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
				fnMesh.closestIntersection(source, ray, NULL, NULL, false, MSpace::kWorld, 99999999.9f, false, &accelParams, intersectionFloat, &hitRayParam, &hitFace, &hitTriangle, &hitBary1, &hitBary2, 0.01f, &status);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				fnMesh.getConnectedShaders(path.instanceNumber(), shaders, shIndices);

				m_shader = (shIndices[hitFace] >= 0) ? shaders[shIndices[hitFace]] : MObject::kNullObj;
			}
			else if (path.apiType() == MFn::kNurbsSurface) {
				MFnNurbsSurface fnNurbs(path, &status);
				CHECK_MSTATUS_AND_RETURN_IT(status);

				// Get nurbs intersections and use the closest
				double u, v;
				fnNurbs.intersect(source, ray, u, v, pivot, 0.01, MSpace::kWorld);

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

				fnNurbs.getConnectedShaders(path.instanceNumber(), shaders, shIndices);

				int patchIdx = patchId[0] + patchId[1] * fnNurbs.numSpansInU();
				m_shader = (shIndices[patchIdx] >= 0) ? shaders[shIndices[patchIdx]] : MObject::kNullObj;
			}
			MGlobal::executeCommand("materialPickerCtxValues (`currentCtx`);");
		}
		else if (event.mouseButton() == MEvent::kMiddleMouse) {
			if (!m_shader.isNull()) {
				MFnDependencyNode fnShader(m_shader);
				MGlobal::executeCommand("hyperShade -assign " + fnShader.name(), true, true);
			}
		}
	}

	status = m_selectionState.restoreSelection();
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;
}