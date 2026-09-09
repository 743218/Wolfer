#pragma once
#include "../../FuncHook.h"

class ActorOnPushHook : public FuncHook {
private:
	using func_t = bool(__thiscall*)(Actor*);
	static inline func_t oFunc;

	static bool ActorOnPushHookCallback(Actor* _this) {
    return false; //LEL
		return oFunc(_this);
	}
public:
	ActorOnPushHook() {
		OriginFunc = (void*)&oFunc;
		func = (void*)&ActorOnPushHookCallback;
	}
};
