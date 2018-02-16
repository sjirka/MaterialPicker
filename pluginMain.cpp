#include "MaterialPickerCtxCmd.h"
#include "MaterialPickerCmd.h"

#include <maya\MFnPlugin.h>
#include <maya\MGlobal.h>


MStatus initializePlugin(MObject obj)
{
	MStatus status;

	MFnPlugin fnPlugin(obj, "Stepan Jirka", "1.0", "Any");

	status = fnPlugin.registerContextCommand("materialPickerCtx", MaterialPickerCtxCmd::creator);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnPlugin.registerCommand("materialPicker", MaterialPickerCmd::creator, MaterialPickerCmd::newSyntax);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MGlobal::executeCommand("if(!`runTimeCommand -ex materialPicker_pressed`)runTimeCommand -ann \"Activate materialPickerCtx\" -cat \"Shader tools\" -d 0 -c \"materialPicker;\" materialPicker_pressed;");
	MGlobal::executeCommand("if(!`runTimeCommand -ex materialPicker_released`)runTimeCommand -ann \"Deactivate materialPickerCtx\" -cat \"Shader tools\" -d 0 -c \"materialPicker -ex;\" materialPicker_released;");

	return MS::kSuccess;
}

MStatus uninitializePlugin(MObject obj)
{
	MStatus status;

	MFnPlugin fnPlugin(obj);

	status = fnPlugin.deregisterContextCommand("materialPickerCtx");
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnPlugin.deregisterCommand("materialPicker");
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;
}
