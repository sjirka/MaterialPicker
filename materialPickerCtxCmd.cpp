#include "MaterialPickerCtxCmd.h"

MaterialPickerCtxCmd::MaterialPickerCtxCmd(){
}

MaterialPickerCtxCmd::~MaterialPickerCtxCmd(){
}

void *MaterialPickerCtxCmd::creator() {
	return new MaterialPickerCtxCmd;
}

MPxContext* MaterialPickerCtxCmd::makeObj() {
	return new MaterialPickerCtx;
}