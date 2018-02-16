#pragma once

#include <maya\MPxCommand.h>

#define EXIT_FLAG			"-ex"
#define EXIT_FLAG_LONG		"-exit"

class MaterialPickerCmd : public MPxCommand
{
public:
	MaterialPickerCmd();
	virtual ~MaterialPickerCmd();

	static void *creator();

	static MSyntax newSyntax();

	virtual MStatus doIt(const MArgList& argList);

	virtual bool isUndoable() const;

	static MString activeCtx;
};

