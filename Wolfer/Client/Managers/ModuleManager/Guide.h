#pragma once

/**

===============================
       Module Creation Guide
===============================

This guide outlines the basic structure and best practices for creating modules
in the client. Reference modules: Aura, Fly

-------------------------------
1. Module Class Structure
-------------------------------

- Every module should inherit from `Module`.
- Define the constructor to initialize name, description, and category.
- Override relevant methods: `onEnable`, `onTick`, `onDisable`, etc.
- Register settings inside the constructor.

Example:

class MyModule : public Module {
public:
    MyModule() : Module("MyModule", "Does something cool", Category::MISC) {
        registerSetting(new BoolSetting("Enabled", "Toggle feature", &enabled, true));
    }

    void onTick(GameMode* gm) override {
        // Logic here
    }

private:
    bool enabled = true;
};

-------------------------------
2. Settings
-------------------------------

Register settings in the constructor to expose module customization to the user.

Types:
- BoolSetting (on/off)
- SliderSetting<float> (decimal range)
- SliderSetting<int> (integer range)
- EnumSetting (option list)

Example:

registerSetting(new SliderSetting<float>("Speed", "Move speed", &speed, 1.0f, 0.1f, 5.0f));

-------------------------------
3. Overridable Hooks
-------------------------------

Useful overrides for modules:
- `onEnable()` – Called when the module is toggled on
- `onDisable()` – Called when toggled off
- `onTick(GameMode*)` – Called every tick while active
- `onSendPacket(Packet*)` – Modify packets (e.g., rotation spoof)
- `onUpdateRotation(LocalPlayer*)` – Called when game updates player rotation
- `onD2DRender()` – For GUI render overlays (if needed)
- `onMCRender(MinecraftRenderContext*)` – For minecraft render overlays (if needed)
- `onLevelRender()` – For 3D rendering

-------------------------------
4. Utility Access
-------------------------------

Use these utilities for accessing data:

- g_Data.getLocalPlayer()
- g_Data.canUseMoveKeys()
- g_Data.isKeyDown(VK_SPACE) // or 'W', 'A', 'S', 'D'
- WorldUtil::distanceToEntity(...)
- TargetUtil::isTargetValid(...)

-------------------------------
5. Motion & Rotation
-------------------------------

To set player movement:

Vector3<float> motion(0.f, 0.f, 0.f);
player->lerpMotion(motion);

To rotate player view:

Vector2<float> rot = eyePos.CalcAngle(targetPos);
player->rotation->presentRot = rot;

To apply rotation in packets:

if (packet->getId() == PacketID::PlayerAuthInput) {
    PlayerAuthInputPacket* p = static_cast<PlayerAuthInputPacket*>(packet);
    p->rotation.x = rot.x;
    p->rotation.y = rot.y;
    p->headYaw = rot.y;
}

-------------------------------
6. Performance Tips
-------------------------------

- Avoid unnecessary math every tick.
- Check `if (!localPlayer) return;` early to avoid null access.
- Clear any vectors used per tick (like targets) to avoid memory bloat.
- Use `tickCounter` for delaying logic rather than running code every tick.

-------------------------------
7. Example Tick Logic
-------------------------------

void onTick(GameMode* gm) override {
    LocalPlayer* player = g_Data.getLocalPlayer();
    if (!player) return;

    if (++tickCounter < delay) return;
    tickCounter = 0;

    // Do something
}

-------------------------------
8. Clean Code Practices
-------------------------------

- Use private members for settings.
- Keep module logic modular and readable.
- Use meaningful variable names.
- Do NOT use comments.
- ONLY use .h, no .cpp

-------------------------------
9. Don't Forget!
-------------------------------

- Call `targets.clear();` at the end of each tick if using lists. [changed targets to your list name]
- Always check if a pointer (like entity or player) is null before using it.
- Avoid using raw magic numbers—use named variables for settings or constants.

Happy hacking!

**/
