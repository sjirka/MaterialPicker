#pragma once

#include "MaterialPickerCtx.h"

#include <maya\MPxContextCommand.h>

class MaterialPickerCtxCmd : public MPxContextCommand
{
public:
	MaterialPickerCtxCmd();
	virtual ~MaterialPickerCtxCmd();

	static void *creator();
	virtual MPxContext* makeObj();
};