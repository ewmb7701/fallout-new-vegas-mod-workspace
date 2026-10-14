#include "on_enemy_health_target_change.h"
#include "utility.h"
#include "nvse/GameUI.h"

namespace
{
  constexpr char kEventName[] = "OnEnemyHealthTargetChange";
  NVSEEventManagerInterface *nvseEventManager = nullptr;

  bool HandleResult(NVSEEventManagerInterface::ResultElement &result, void *data)
  {
    if (result.type == 1 && result.number == 0)
      *static_cast<bool *>(data) = true;
    return true;
  }

  bool __stdcall AllowEnemyHealthTarget(TESObjectREFR *target)
  {
    return target == GetEnemyHealthTarget() || OnEnemyHealthTargetChange::AllowChange(target);
  }

  // The damaged-actor caller ignores the setter's outcome and draws its local
  // actor anyway. Only continue if that actor became the actual HUD health-bar target.
  __declspec(naked) void DamagedActorEnemyHealthContinuationHook()
  {
    __asm {
      add esp, 4 // displaced caller argument cleanup
      pushfd
      push eax
      mov eax, dword ptr [0x11D96C0]
      mov eax, [eax + 0x21C]
      cmp eax, [ebp - 8]
      jne cancelled
      pop eax
      popfd
      push 1 // displaced visible value
      push 0x772ECC
      ret
    cancelled:
      pop eax
      popfd
      push 0x772FA1 // native friendly-bar hide path and common state updates
      ret
    }
  }

  // VATS sets its show-bar flag before calling the setter. A veto leaving no
  // target must also take the native hide path instead of showing stale pips.
  // Hook the shared jump itself: other VATS branches enter at 770DF5, after
  // the setter caller's argument cleanup at 770DF2.
  __declspec(naked) void VATSEnemyHealthContinuationHook()
  {
    __asm {
      pushfd
      push eax
      mov eax, dword ptr [0x11D96C0]
      cmp dword ptr [eax + 0x21C], 0
      je cancelled
      pop eax
      popfd
      push 0x770F3C // displaced jump; update draws the retained/accepted target
      ret
    cancelled:
      pop eax
      popfd
      mov byte ptr [ebp - 0x15], 0
      push 0x770F00 // native bar/name/meter hide path
      ret
    }
  }

  // Hook the resolved target stores, including the separate VATS branch.
  // Damage can change the HUD health-bar target without changing PlayerCharacter::D44.
  __declspec(naked) void EnemyHealthTargetHook()
  {
    __asm {
      pushfd
      pushad
      push edx
      call AllowEnemyHealthTarget
      test al, al
      jz cancelled
      popad
      popfd
      mov [ecx + 0x21C], edx
      push 0x773243
      ret
    cancelled:
      popad
      popfd
      push 0x7732A9
      ret
    }
  }

  __declspec(naked) void VATSEnemyHealthTargetHook()
  {
    __asm {
      pushfd
      pushad
      push eax
      call AllowEnemyHealthTarget
      test al, al
      jz cancelled
      popad
      popfd
      mov [edx + 0x21C], eax
      push 0x773243
      ret
    cancelled:
      popad
      popfd
      push 0x7732A9
      ret
    }
  }
}

bool OnEnemyHealthTargetChange::AllowChange(TESObjectREFR *target)
{
  bool cancel = false;
  nvseEventManager->DispatchEventAlt(kEventName, HandleResult, &cancel, nullptr, target);
  return !cancel;
}

void OnEnemyHealthTargetChange::RegisterEvent(NVSEEventManagerInterface *eventInterface)
{
  nvseEventManager = eventInterface;
  static NVSEEventManagerInterface::ParamType params[] = {
      NVSEEventManagerInterface::eParamType_Reference};
  nvseEventManager->RegisterEvent(kEventName, 1, params,
                                  NVSEEventManagerInterface::kFlag_FlushOnLoad);
}

void OnEnemyHealthTargetChange::InstallHook()
{
  WriteRelJump(0x77323D, reinterpret_cast<uintptr_t>(&EnemyHealthTargetHook));
  WriteRelJump(0x77322C, reinterpret_cast<uintptr_t>(&VATSEnemyHealthTargetHook));
  WriteRelJump(0x772EC7, reinterpret_cast<uintptr_t>(&DamagedActorEnemyHealthContinuationHook));
  WriteRelJump(0x770DF5, reinterpret_cast<uintptr_t>(&VATSEnemyHealthContinuationHook));
}
