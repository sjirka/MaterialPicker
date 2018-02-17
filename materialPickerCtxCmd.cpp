#include "MaterialPickerCtxCmd.h"
#include <maya\MFnDependencyNode.h>

MaterialPickerCtxCmd::MaterialPickerCtxCmd(){
}

MaterialPickerCtxCmd::~MaterialPickerCtxCmd(){
}

void *MaterialPickerCtxCmd::creator() {
	return new MaterialPickerCtxCmd;
}

MPxContext* MaterialPickerCtxCmd::makeObj() {
	return m_context = new MaterialPickerCtx;
}

MStatus MaterialPickerCtxCmd::appendSyntax() {
	MStatus status(MStatus::kSuccess);

	MSyntax mySyntax = syntax(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	mySyntax.addFlag(SHADER_FLAG, SHADER_FLAG_LONG, MSyntax::kString);

	return MS::kSuccess;
}

MStatus MaterialPickerCtxCmd::doQueryFlags() {
	MStatus status(MStatus::kSuccess);

	MArgParser argData = parser();

	if (argData.isFlagSet(SHADER_FLAG)) {
		if (!m_context->m_shader.isNull()) {
			MFnDependencyNode fnShader(m_context->m_shader);
			setResult(fnShader.name());
		}
	}

	return status;
}