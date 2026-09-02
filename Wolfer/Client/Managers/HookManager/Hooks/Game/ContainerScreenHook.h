#pragma once
#include "../FuncHook.h"
#include "ContainerTickController.h"

class ContainerScreenTickHook : public FuncHook { //no longer a broken piece of mess!
private:
	using func_t = __int64(__fastcall*)(ContainerScreenController*);
	static inline func_t oFunc;
	static __int64 callback(ContainerScreenController* _this) {
		if (g_Data.getLocalPlayer() && _this) {
			ModuleManager::onContainerTick(_this);
		}
		return oFunc(_this);
	}
public:
	ContainerScreenTickHook() {
		OriginFunc = (void**)&oFunc;
		func = (void*)&callback;
	}
};

