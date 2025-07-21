#include "ItemESP.h"
#include <cmath>

void ItemESP::onLevelRender() {
    LocalPlayer* localPlayer = g_Data.getLocalPlayer();
    if (!localPlayer) return;

    Level* level = localPlayer->level;
    if (!level) return;

    Vector3<float> playerPos = localPlayer->getPos();
    
    // Calculate pulsing effect
    static float timeAccumulator = 0.0f;
    timeAccumulator += MCR::deltaTime;
    float pulseMultiplier = 1.0f;
    if (enablePulse) {
        pulseMultiplier = 0.5f + 0.5f * sinf(timeAccumulator * pulseSpeed);
    }

    // Get all actors in the level
    for (auto entity : level->getRuntimeActorList()) {
        if (!entity) continue;

        // Check if it's an item entity (entity ID 64 is typically items)
        uint32_t entityId = entity->getActorTypeComponent()->id;
        if (entityId != 64) continue; // 64 is the ID for item entities

        Vector3<float> entityPos = entity->getPos();
        float dx = entityPos.x - playerPos.x;
        float dy = entityPos.y - playerPos.y;
        float dz = entityPos.z - playerPos.z;
        float distSq = dx * dx + dy * dy + dz * dz;

        // Don't render items too far away (100 blocks)
        if (distSq > 10000.0f) continue;

        // Get entity's bounding box
        AABB entityAABB = entity->aabbShape->aabb;
        
        // Apply pulsing to the glow intensity
        float currentIntensity = glowIntensity * pulseMultiplier;
        
        // Create dynamic color with pulsing
        WolferColor dynamicColor(
            color.r, 
            color.g, 
            color.b, 
            static_cast<int>(255 * currentIntensity)
        );

        // Render based on shader mode
        switch (shaderMode) {
            case 0: // Glow only
                renderItemGlow(entityPos, entityAABB, dynamicColor, currentIntensity);
                break;
            case 1: // Outline only
                renderItemOutline(entityPos, entityAABB, dynamicColor);
                break;
            case 2: // Both
                renderItemGlow(entityPos, entityAABB, dynamicColor, currentIntensity);
                renderItemOutline(entityPos, entityAABB, dynamicColor);
                break;
        }
    }
}

void ItemESP::renderItemGlow(const Vector3<float>& itemPos, const AABB& itemAABB, const WolferColor& glowColor, float intensity) {
    if (!MCR::tessellator || !MCR::screenCtx || !MCR::blendMaterial) return;

    Vector3<float> origin = MCR::origin;
    Vector3<float> localPos = Vector3<float>(
        itemPos.x - origin.x,
        itemPos.y - origin.y, 
        itemPos.z - origin.z
    );

    // Calculate item dimensions
    Vector3<float> size = Vector3<float>(
        itemAABB.upper.x - itemAABB.lower.x,
        itemAABB.upper.y - itemAABB.lower.y,
        itemAABB.upper.z - itemAABB.lower.z
    );

    // Create multiple layers for glow effect
    const int glowLayers = 3;
    for (int layer = 0; layer < glowLayers; layer++) {
        float layerScale = 1.0f + (layer * 0.2f * intensity);
        float layerAlpha = (glowColor.a / 255.0f) * (1.0f - (layer * 0.3f));
        
        WolferColor layerColor(
            glowColor.r, 
            glowColor.g, 
            glowColor.b, 
            static_cast<int>(255 * layerAlpha)
        );

        MCR::setColor(layerColor);
        MCR::tessellator->begin(VertextFormat::TRIANGLE_STRIP);

        // Render expanded cube for glow effect
        Vector3<float> expandedSize = Vector3<float>(size.x * layerScale, size.y * layerScale, size.z * layerScale);
        Vector3<float> halfSize = Vector3<float>(expandedSize.x * 0.5f, expandedSize.y * 0.5f, expandedSize.z * 0.5f);

        // Front face
        MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y - halfSize.y, localPos.z + halfSize.z);
        MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y - halfSize.y, localPos.z + halfSize.z);
        MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y + halfSize.y, localPos.z + halfSize.z);
        MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y + halfSize.y, localPos.z + halfSize.z);

        MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);

        // Back face
        MCR::tessellator->begin(VertextFormat::TRIANGLE_STRIP);
        MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y - halfSize.y, localPos.z - halfSize.z);
        MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y - halfSize.y, localPos.z - halfSize.z);
        MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y + halfSize.y, localPos.z - halfSize.z);
        MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y + halfSize.y, localPos.z - halfSize.z);

        MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);

        // Left face
        MCR::tessellator->begin(VertextFormat::TRIANGLE_STRIP);
        MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y - halfSize.y, localPos.z - halfSize.z);
        MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y - halfSize.y, localPos.z + halfSize.z);
        MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y + halfSize.y, localPos.z - halfSize.z);
        MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y + halfSize.y, localPos.z + halfSize.z);

        MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);

        // Right face
        MCR::tessellator->begin(VertextFormat::TRIANGLE_STRIP);
        MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y - halfSize.y, localPos.z + halfSize.z);
        MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y - halfSize.y, localPos.z - halfSize.z);
        MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y + halfSize.y, localPos.z + halfSize.z);
        MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y + halfSize.y, localPos.z - halfSize.z);

        MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);

        // Top face
        MCR::tessellator->begin(VertextFormat::TRIANGLE_STRIP);
        MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y + halfSize.y, localPos.z + halfSize.z);
        MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y + halfSize.y, localPos.z + halfSize.z);
        MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y + halfSize.y, localPos.z - halfSize.z);
        MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y + halfSize.y, localPos.z - halfSize.z);

        MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);

        // Bottom face
        MCR::tessellator->begin(VertextFormat::TRIANGLE_STRIP);
        MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y - halfSize.y, localPos.z - halfSize.z);
        MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y - halfSize.y, localPos.z - halfSize.z);
        MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y - halfSize.y, localPos.z + halfSize.z);
        MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y - halfSize.y, localPos.z + halfSize.z);

        MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);
    }
}

void ItemESP::renderItemOutline(const Vector3<float>& itemPos, const AABB& itemAABB, const WolferColor& outlineColor) {
    if (!MCR::tessellator || !MCR::screenCtx || !MCR::blendMaterial) return;

    Vector3<float> origin = MCR::origin;
    Vector3<float> localPos = Vector3<float>(
        itemPos.x - origin.x,
        itemPos.y - origin.y, 
        itemPos.z - origin.z
    );

    // Calculate item dimensions
    Vector3<float> size = Vector3<float>(
        itemAABB.upper.x - itemAABB.lower.x,
        itemAABB.upper.y - itemAABB.lower.y,
        itemAABB.upper.z - itemAABB.lower.z
    );

    Vector3<float> halfSize = Vector3<float>(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);

    MCR::setColor(outlineColor);
    MCR::tessellator->begin(VertextFormat::LINE_LIST);

    // Draw wireframe cube outline
    Vector3<float> vertices[8] = {
        { localPos.x - halfSize.x, localPos.y - halfSize.y, localPos.z - halfSize.z }, // 0
        { localPos.x + halfSize.x, localPos.y - halfSize.y, localPos.z - halfSize.z }, // 1
        { localPos.x + halfSize.x, localPos.y + halfSize.y, localPos.z - halfSize.z }, // 2
        { localPos.x - halfSize.x, localPos.y + halfSize.y, localPos.z - halfSize.z }, // 3
        { localPos.x - halfSize.x, localPos.y - halfSize.y, localPos.z + halfSize.z }, // 4
        { localPos.x + halfSize.x, localPos.y - halfSize.y, localPos.z + halfSize.z }, // 5
        { localPos.x + halfSize.x, localPos.y + halfSize.y, localPos.z + halfSize.z }, // 6
        { localPos.x - halfSize.x, localPos.y + halfSize.y, localPos.z + halfSize.z }  // 7
    };

    // Bottom face edges
    MCR::tessellator->vertex(vertices[0].x, vertices[0].y, vertices[0].z);
    MCR::tessellator->vertex(vertices[1].x, vertices[1].y, vertices[1].z);
    MCR::tessellator->vertex(vertices[1].x, vertices[1].y, vertices[1].z);
    MCR::tessellator->vertex(vertices[2].x, vertices[2].y, vertices[2].z);
    MCR::tessellator->vertex(vertices[2].x, vertices[2].y, vertices[2].z);
    MCR::tessellator->vertex(vertices[3].x, vertices[3].y, vertices[3].z);
    MCR::tessellator->vertex(vertices[3].x, vertices[3].y, vertices[3].z);
    MCR::tessellator->vertex(vertices[0].x, vertices[0].y, vertices[0].z);

    // Top face edges
    MCR::tessellator->vertex(vertices[4].x, vertices[4].y, vertices[4].z);
    MCR::tessellator->vertex(vertices[5].x, vertices[5].y, vertices[5].z);
    MCR::tessellator->vertex(vertices[5].x, vertices[5].y, vertices[5].z);
    MCR::tessellator->vertex(vertices[6].x, vertices[6].y, vertices[6].z);
    MCR::tessellator->vertex(vertices[6].x, vertices[6].y, vertices[6].z);
    MCR::tessellator->vertex(vertices[7].x, vertices[7].y, vertices[7].z);
    MCR::tessellator->vertex(vertices[7].x, vertices[7].y, vertices[7].z);
    MCR::tessellator->vertex(vertices[4].x, vertices[4].y, vertices[4].z);

    // Vertical edges
    MCR::tessellator->vertex(vertices[0].x, vertices[0].y, vertices[0].z);
    MCR::tessellator->vertex(vertices[4].x, vertices[4].y, vertices[4].z);
    MCR::tessellator->vertex(vertices[1].x, vertices[1].y, vertices[1].z);
    MCR::tessellator->vertex(vertices[5].x, vertices[5].y, vertices[5].z);
    MCR::tessellator->vertex(vertices[2].x, vertices[2].y, vertices[2].z);
    MCR::tessellator->vertex(vertices[6].x, vertices[6].y, vertices[6].z);
    MCR::tessellator->vertex(vertices[3].x, vertices[3].y, vertices[3].z);
    MCR::tessellator->vertex(vertices[7].x, vertices[7].y, vertices[7].z);

    MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);
}