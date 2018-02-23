#pragma once

#include <maya\MPxContext.h>
#include <maya\MGlobal.h>
#include <maya\MRichSelection.h>
#include <maya\MSelectionList.h>
#include <maya\MGlobal.h>
#include "../_library/SSelection.h"

#define helpOptionVar "materialPickerDisplayHelp"

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

	MObject
		m_shader,
		m_shaderTemp;
	bool m_displayHelp;
private:
	SSelection m_selectionState;
	MSelectionList m_activeSelection;
};

