#include "ActorUtils.h"

#include "../../Game.h"
#include "../../World/Actor.h"
#include "TargetUtil.h"

class ActorOwnerComponent
{
public:
    class Actor* mActor;
};

struct ActorUniqueIDComponent {
    uint64_t mUniqueID;
};

std::vector<Actor*> ActorUtils::getActorList(bool mobs, bool friendCheck) {
    std::vector<Actor*> result;

    Actor* localPlayer = g_Data.getClientInstance()->getLocalPlayer();
    if (!localPlayer) return result;

    auto* context = localPlayer->getEntityContext();
    auto& registry = context->getRegistry();

    auto view = registry.view<ActorOwnerComponent, ActorTypeComponent>();

    for (auto entity : view) {
        auto& owner = view.get<ActorOwnerComponent>(entity);
        auto& type = view.get<ActorTypeComponent>(entity);

        if (!owner.mActor) continue;
        if (!TargetUtil::isTargetValid(owner.mActor, mobs, friendCheck)) continue;

        result.push_back(owner.mActor);
    }

    return result;
}

std::vector<Actor*> ActorUtils::getActorsOfType(ActorType type) {
    std::vector<Actor*> result;

    Actor* localPlayer = g_Data.getClientInstance()->getLocalPlayer();
    if (!localPlayer) return result;

    auto* context = localPlayer->getEntityContext();
    auto& registry = context->getRegistry();

    auto view = registry.view<ActorOwnerComponent, ActorTypeComponent>();

    for (auto entity : view) {
        auto& owner = view.get<ActorOwnerComponent>(entity);
        auto& typeComponent = view.get<ActorTypeComponent>(entity);

        if (!owner.mActor) continue;
        if (typeComponent.id != static_cast<int>(type)) continue;

        result.push_back(owner.mActor);
    }

    return result;
}
