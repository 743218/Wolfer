#pragma once
#include "../FuncHook.h"

class GetFovHook : public FuncHook {
private:
	using func_t = float(__thiscall*)(LevelRendererPlayer*, float, bool);
	static inline func_t oFunc;

    static float GetFovCallback(LevelRendererPlayer* _this, float a, bool applyEffects) {
        static void* getFovForCameraReceive = (void*)Memory::findSig("F3 0F 59 05 ?? ?? ?? ?? 48 8B C7 48 8B 5C 24");
        if (_ReturnAddress() == getFovForCameraReceive) {
            static Zoom* zoomMod = ModuleManager::getModule<Zoom>();
            if (zoomMod) {
                float originalFov = oFunc(_this, a, applyEffects);
                float& currentFov = zoomMod->currentFov;
                float targetFov = zoomMod->isEnabled() ? zoomMod->zoomFov : originalFov;

                float smoothing = zoomMod->zoomSpeed * 0.66f;

                if (currentFov < targetFov) {
                    currentFov = std::min(currentFov + smoothing, targetFov);
                }
                else if (currentFov > targetFov) {
                    currentFov = std::max(currentFov - smoothing, targetFov);
                }

                return currentFov;
            }
        }

        return oFunc(_this, a, applyEffects);
    }
public:
	GetFovHook() {
		OriginFunc = (void*)&oFunc;
		func = (void*)&GetFovCallback;
	}
};