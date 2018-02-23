#include "MaterialPickerCtx.h"
#include "../_library/SCamera.h"
#include "materialPickerCursor.h"

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

#define helpString "Pick shader from geometry with left mouse button. Assign picked shader with middle mouse button"

MaterialPickerCtx::MaterialPickerCtx(){
	setTitleString("Material Picker");
	setImage("materialPicker.xpm", MPxContext::kImage1);

	MCursor materialPickerCursor(materialPicker_width, materialPicker_height, materialPicker_x_hot, materialPicker_y_hot, materialPicker_bits, materialPicker_transp_bits);
	setCursor(materialPickerCursor);

	MSelectionList initialShadingGroups;
	MGlobal::getSelectionListByName("initialShadingGroup", initialShadingGroups);
	if (!initialShadingGroups.isEmpty())
		initialShadingGroups.getDependNode(0, m_shader);

	m_displayHelp = (!MGlobal::optionVarExists(helpOptionVar) || (MGlobal::optionVarExists(helpOptionVar) && MGlobal::optionVarIntValue(helpOptionVar) == 1)) ? true : false;
}

MaterialPickerCtx::~MaterialPickerCtx()
{
}

void MaterialPickerCtx::toolOnSetup(MEvent &event) {
	setHelpString(helpString);

	if (m_displayHelp)
		MGlobal::executeCommand("inViewMessage -fst 4000 -smg \"Pick shader from geometry with <span style = 'color:#F4FA58;'>left mouse button</span>. Assign picked shader with <span style='color:#F4FA58;'>middle mouse button</span>\" -pos topCenter -fade;");
}

void MaterialPickerCtx::doEnterRegion() {
	setHelpString(helpString);
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
		materialPicker_transp_bits);
	setCursor(materialPickerCursor);

	status = m_selectionState.storeCurrentSelection();
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = MGlobal::setHiliteList(MSelectionList());
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Click select geometry
	short x, y;
	event.getPosition(x, y);
	MGlobal::selectFromScreen(x, y, MGlobal::kReplaceList, MGlobal::selectionMethod());

	MGlobal::getActiveSelectionList(m_activeSelection);

	status = m_selectionState.restoreSelection();
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return doDrag(event);
}

MStatus MaterialPickerCtx::doDrag(MEvent &event) {
	MStatus status;

	if (m_activeSelection.isEmpty() || event.mouseButton() != MEvent::kLeftMouse)
		return MS::kSuccess;

	m_shaderTemp = MObject::kNullObj;

	MDagPath path;
	status = m_activeSelection.getDagPath(0, path);
	CHECK_MSTATUS_AND_RETURN_IT(status)
	status = path.extendToShape();
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Get the view based ray
	short x, y;
	event.getPosition(x, y);

	MPoint source;
	MVector ray;
	MObjectArray shaders;
	MIntArray shIndices;
	bool hit = false;
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

		if (hit) {
			fnMesh.getConnectedShaders(path.instanceNumber(), shaders, shIndices);
			m_shaderTemp = (shIndices[hitFace] >= 0) ? shaders[shIndices[hitFace]] : MObject::kNullObj;
		}
	}
	else if (path.apiType() == MFn::kNurbsSurface) {
		MFnNurbsSurface fnNurbs(path, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Get nurbs intersections and use the closest
		double u, v;
		hit = fnNurbs.intersect(source, ray, u, v, MPoint(), 0.01, MSpace::kWorld);

		if (hit) {
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
			m_shaderTemp = (shIndices[patchIdx] >= 0) ? shaders[shIndices[patchIdx]] : MObject::kNullObj;
		}
	}

	return MS::kSuccess;
}

MStatus MaterialPickerCtx::doRelease(MEvent &event){
	MStatus status;

	MCursor materialPickerCursor(materialPicker_width, materialPicker_height, materialPicker_x_hot, materialPicker_y_hot, materialPicker_bits, materialPicker_transp_bits);
	setCursor(materialPickerCursor);
	
	if (event.mouseButton() == MEvent::kLeftMouse && !m_shaderTemp.isNull()) {
		m_shader = m_shaderTemp;
		MGlobal::executeCommand("materialPickerCtxValues (`currentCtx`);");
	}
	else if (event.mouseButton() == MEvent::kMiddleMouse) {
		if (!m_shader.isNull()) {
			short x, y;
			event.getPosition(x, y);
			MGlobal::selectFromScreen(x, y, MGlobal::kReplaceList, MGlobal::selectionMethod());

			MSelectionList selection;
			MGlobal::getActiveSelectionList(selection);

			if (!selection.isEmpty()) {
				MFnDependencyNode fnShader(m_shader);
				MGlobal::executeCommand("hyperShade -assign " + fnShader.name(), true, true);
			}

			status = m_selectionState.restoreSelection();
			CHECK_MSTATUS_AND_RETURN_IT(status);
		}
	}

	return MS::kSuccess;
}