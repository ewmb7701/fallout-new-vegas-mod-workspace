#pragma once
#include "game.h"

namespace Repair {
extern Game::Form* armorKit;
inline float RepairFraction(float skill,bool fallout3) {
    skill=std::clamp(skill,0.0f,100.0f);
    if (fallout3) return (10.0f+0.2f*skill)*0.01f;
    return skill<25 ? 0.10f : skill<50 ? 0.15f : skill<75 ? 0.20f : skill<100 ? 0.25f : 0.30f;
}
inline float RepairedCondition(float condition,float fraction) {
    return condition<99.0f ? std::clamp(condition*0.01f+fraction,0.0f,1.0f) : 0.0f;
}
Game::Form* KitFor(Game::Entry* entry);
bool __stdcall IsKit(Game::Form* form);
bool Install();
const char* Failure();
}
