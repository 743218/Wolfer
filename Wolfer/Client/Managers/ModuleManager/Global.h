#pragma once

class Global {
public:
    inline static Vector2<float> rotation = { 0.f, 0.f };
    inline static std::mutex rotationMutex;

    static void addRotation(const Vector2<float>& delta) {
        std::lock_guard<std::mutex> lock(rotationMutex);
        rotation.x += delta.x;
        rotation.y += delta.y;
    }

    static void resetRotation() {
        std::lock_guard<std::mutex> lock(rotationMutex);
        rotation.x = 0.0f;
        rotation.y = 0.0f;
    }
};