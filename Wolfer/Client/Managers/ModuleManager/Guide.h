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

/**
===============================
         Bypassing Guide
===============================

This guide covers common concepts and tips for bypassing basic anti-cheat
mechanisms when creating client modules. Use responsibly.

-------------------------------
1. Understand the Anti-Cheat
-------------------------------

- Research what type of anti-cheat the server uses (e.g., checks for speed hacks, rotation spoofing, packet anomalies).
- Identify which packets or actions trigger detection.
- Learn if the anti-cheat uses server-side prediction, velocity checks, or packet timing analysis.

-------------------------------
2. Packet Manipulation
-------------------------------

- Modify outgoing packets subtly to avoid detection.
- Avoid sending impossible values (e.g., too fast or instant rotation changes).
- Smooth out changes over multiple ticks instead of instant.
- Use `onSendPacket(Packet*)` to intercept and alter packets carefully.

Example:

    void onSendPacket(Packet* packet) override {
        if (packet->getId() == PacketID::PlayerAuthInput) {
            PlayerAuthInputPacket* p = (PlayerAuthInputPacket*)packet;
            // Clamp rotation changes within reasonable limits
            p->rotation.x = ClampRotation(p->rotation.x);
            p->rotation.y = ClampRotation(p->rotation.y);
        }
    }

-------------------------------
3. Timing and Delays
-------------------------------

- Use tick counters or timers to avoid performing suspicious actions every tick.
- Introduce random delays or jitter in actions.
- Avoid repetitive, perfectly timed patterns (e.g., clicking exactly every 50 ms).

-------------------------------
4. Movement & Rotation Limits
-------------------------------

- Do not exceed natural player movement limits.
- Limit speed increases to a realistic range.
- When changing rotation, do it gradually over multiple ticks.

-------------------------------
5. Anti-Detection Practices
-------------------------------

- Avoid calling suspicious functions if the player is not in a valid state.
- Null-check all pointers before usage.
- Clear or reset variables like target lists each tick.
- Avoid direct memory patches or unsafe hooks unless you understand the consequences.

-------------------------------
6. Use "Soft" Features First
-------------------------------

- Prefer manipulating player input rather than raw packets.
- Try to work with natural game events (jump events, movement keys) to blend in.
- Avoid blatant cheats that drastically change player state instantly.

-------------------------------
7. Testing & Debugging
-------------------------------

- Test modules on non-cheat or low-strictness servers first.
- Log suspicious events and packet modifications.
- Adjust parameters dynamically to find safe thresholds.

-------------------------------
8. Example Bypass Snippet
-------------------------------

    int tickCounter = 0;
    const int delayTicks = 5;

    void onTick(GameMode* gm) override {
        LocalPlayer* player = g_Data.getLocalPlayer();
        if (!player) return;

        if (++tickCounter < delayTicks) return;
        tickCounter = 0;

        // Gradually modify motion for subtle speed change
        Vector3<float> motion = player->getVelocity();
        motion.x = SmoothChange(motion.x, targetSpeedX, 0.05f);
        motion.z = SmoothChange(motion.z, targetSpeedZ, 0.05f);
        player->lerpMotion(motion);
    }

-------------------------------
9. Final Notes
-------------------------------

- Never blindly copy bypass methods; adapt them per server.
- Prioritize subtlety and natural behavior.
- Keep updating your bypasses as anti-cheat evolves.
- Always respect server rules and terms.

Good luck and hack smartly!

**/
/*
============================== =
      HOW 2 GET BITCHES
============================== =

1. Be confident, but not cringe
   > No one likes the guy shouting “IM THE KING” every 5 sec.Chill and flex lowkey.

    2. Hygiene matters, fam
   > Smell good or at least don’t smell like expired mayo.Shower > 0

    3. Dress like you give a * tiny * damn
   > You don’t gotta drip Gucci, just don’t wear socks with sandals.

    4. Have hobbies other than "playing Minecraft at 3 AM"
   > Being passionate about literally anything > being a keyboard warrior.

    5. Respect the heck outta everyone
   > If you’re toxic, you’re out.No one wants to deal with Mr.Salt.

    6. Slide into conversations, not DMs like a creep
   > Start with a "Hey, sup?" not "yo babe wanna marry me?" lol no.

    7. Make 'em laugh or at least smile  
   > Dad jokes, memes, or just weird flexes.Bonus points if you roast yourself.

    8. Be patient, don’t be a thirsty NPC
   > If they don’t reply, maybe they busy, not ghosting you just cuz you exist.

    9. Rejections are just like 404 errors
   > It’s not personal, just move on and update your “player status.”

    10. Have fun, don’t be a tryhard
    > Life’s a game, if you’re not enjoying it, what’s the point ? Chill out.

    Bonus :
    -Don’t call them “bitches” IRL unless you wanna be single forever.
    - Memes > pickup lines 9 / 10 times.
    - If all else fails, blame Discord mods. (jk, pls don’t)

    Peace out and go get those Ws.GG no re.
    */