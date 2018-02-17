#pragma once

#include "MaterialPickerCtx.h"

#include <maya\MPxContextCommand.h>

#define SHADER_FLAG "-sh"
#define SHADER_FLAG_LONG "-shader"

class MaterialPickerCtxCmd : public MPxContextCommand
{
public:
	MaterialPickerCtxCmd();
	virtual ~MaterialPickerCtxCmd();

	static void *creator();
	virtual MPxContext* makeObj();

	virtual MStatus doQueryFlags();
	virtual MStatus appendSyntax();

private:
	MaterialPickerCtx* m_context;
};