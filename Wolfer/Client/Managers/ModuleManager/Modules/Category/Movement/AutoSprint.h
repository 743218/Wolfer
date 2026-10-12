#pragma once
#include "../../ModuleBase/Module.h"

class AutoSprint : public Module {
public:
  AutoSprint();

  void onLevelTick() override {
    auto lp = g_Data.getLocalPlayer();
    if (!lp) return;

    lp->setSprinting(true);
    };
};
