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

	MObject m_shader;
private:
	MString m_helpString;
	
};

