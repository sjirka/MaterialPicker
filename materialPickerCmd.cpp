#include "MaterialPickerCmd.h"

#include <maya\MSyntax.h>
#include <maya\MArgDatabase.h>
#include <maya\MGlobal.h>

MString MaterialPickerCmd::activeCtx = "";

MaterialPickerCmd::MaterialPickerCmd()
{
}


MaterialPickerCmd::~MaterialPickerCmd()
{
}


void *MaterialPickerCmd::creator() {
	return new MaterialPickerCmd;
}

MSyntax MaterialPickerCmd::newSyntax() {
	MSyntax syntax;

	syntax.addFlag(EXIT_FLAG, EXIT_FLAG_LONG, MSyntax::kNoArg);

	syntax.enableEdit(false);
	syntax.enableQuery(false);

	return syntax;
}

MStatus MaterialPickerCmd::doIt(const MArgList& argList) {
	MStatus status;

	MArgDatabase argData(syntax(), argList, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (!argData.isFlagSet(EXIT_FLAG)) {
		status = MGlobal::executeCommand("currentCtx;", activeCtx);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		status = MGlobal::executeCommand("if(!`contextInfo -ex materialPickerCtx`)materialPickerCtx materialPickerCtx; setToolTo materialPickerCtx;");
		CHECK_MSTATUS_AND_RETURN_IT(status);
	}
	else {
		if (activeCtx != "") {
			status = MGlobal::executeCommand("setToolTo " + activeCtx);
			CHECK_MSTATUS_AND_RETURN_IT(status);
			activeCtx = "";
		}
	}
}

bool MaterialPickerCmd::isUndoable() const {
	return false;
}