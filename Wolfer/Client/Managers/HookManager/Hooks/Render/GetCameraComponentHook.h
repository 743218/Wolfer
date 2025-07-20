#pragma once
#include "../FuncHook.h"
#include <libhat/Access.hpp>

class GetCameraComponentHook : public FuncHook {
private:
	using func_t = void* (__thiscall*)(__int64, __int64);
	static inline func_t oFunc;

	static void* GetCameraComponentCallback(__int64 a1, __int64 a2) {
		void* result = oFunc(a1, a2);
		static void* cameraDistanceCall = reinterpret_cast<void*>(Memory::findSig("48 85 C0 74 06 F3 44 0F"));
		if (_ReturnAddress() == cameraDistanceCall && result != nullptr) {

		}
		/*
		void* callFrom = _ReturnAddress();
		if ((uintptr_t)callFrom > Memory::getGameAddress() + 0x43CCDF0 && (uintptr_t)callFrom < Memory::getGameAddress() + 0x43CD146) {
			g_Data.DisplayClientMessage("%llx", callFrom);
			if (result != nullptr) {
				*(float*)((uintptr_t)result + 0x14) = 10.f;
			}
		}
		*/
		return result;
	}
public:
	GetCameraComponentHook() {
		OriginFunc = (void*)&oFunc;
		func = (void*)&GetCameraComponentCallback;
	}
};