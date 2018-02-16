#pragma once

#include <maya\MPxContext.h>
#include <maya\MPoint.h>
#include <maya\MVector.h>
#include <maya\MPointArray.h>
#include <maya\MDagPath.h>

class MaterialPickerCtx : public MPxContext
{
public:
	MaterialPickerCtx();
	virtual ~MaterialPickerCtx();

	virtual void toolOnSetup(MEvent &);
	virtual void doEnterRegion();
	virtual void toolOffCleanup();
	virtual MString stringClassName() const;

	virtual MStatus doPress(MEvent &event);
	virtual MStatus doDrag(MEvent &event);
	virtual MStatus doRelease(MEvent &event);

	// VP2
	virtual MStatus doPress(MEvent &event, MHWRender::MUIDrawManager &drawMgr, const MHWRender::MFrameContext &context);
	virtual MStatus doDrag(MEvent &event, MHWRender::MUIDrawManager &drawMgr, const MHWRender::MFrameContext &context);
	virtual MStatus doRelease(MEvent &event, MHWRender::MUIDrawManager &drawMgr, const MHWRender::MFrameContext &context);
	
	static bool meshClosestIntersection(MDagPath &path, MPoint &source, MVector &ray, MPoint &intersection, MStatus *status = NULL);
	static bool meshClosestIntersection(MObject &mesh, MPoint &source, MVector &ray, MPoint &intersection, MStatus *status = NULL);
	static MPoint closestPoint(MPointArray &cloud, MPoint &toPoint);

private:
	MObject m_shader;
};

