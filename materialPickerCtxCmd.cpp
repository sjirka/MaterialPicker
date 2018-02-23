#include "MaterialPickerCtxCmd.h"
#include <maya\MFnDependencyNode.h>

#include <maya\MToolsInfo.h>

#define SHADER_FLAG "-sh"
#define SHADER_FLAG_LONG "-shader"

#define HELP_FLAG "-hm"
#define HELP_FLAG_LONG "-helpMessage"

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
	mySyntax.addFlag(HELP_FLAG, HELP_FLAG_LONG, MSyntax::kBoolean);

	return MS::kSuccess;
}

MStatus MaterialPickerCtxCmd::doEditFlags() {
	MStatus status(MStatus::kSuccess);

	MArgParser argData = parser();

	if (argData.isFlagSet(HELP_FLAG)) {
		bool value;
		argData.getFlagArgument(HELP_FLAG, 0, value);
		m_context->m_displayHelp = value;
		MGlobal::setOptionVarValue(helpOptionVar, (value) ? 1 : 0);
		MToolsInfo::setDirtyFlag(*m_context);
	}

	return status;
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
	else if (argData.isFlagSet(HELP_FLAG))
			setResult(m_context->m_displayHelp);

	return status;
}