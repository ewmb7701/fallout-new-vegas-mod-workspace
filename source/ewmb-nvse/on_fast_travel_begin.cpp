#include "on_fast_travel_begin.h"
#include "utility.h"
#include "nvse/GameObjects.h"

namespace
{
  constexpr char kFastTravelBeginEventName[] = "OnFastTravelBegin";
  NVSEEventManagerInterface *nvse_event_manager = nullptr;

  bool HandleFastTravelBeginResult(NVSEEventManagerInterface::ResultElement &result, void *data)
  {
    if (result.type == 1 && result.number == 0)
      *static_cast<bool *>(data) = true;
    return true;
  }

  bool __stdcall DispatchFastTravelBegin(PlayerPositionRequest *request)
  {
    // Ordinary position changes use the other branch and do not dispatch.
    if (!request || !request->marker)
      return true;
    bool cancel = false;
    nvse_event_manager->DispatchEventAlt(kFastTravelBeginEventName, HandleFastTravelBeginResult, &cancel,
                                         nullptr, request->marker);
    return !cancel;
  }

  // Shared position-request processor, before marker/linked-ref resolution and JIP OnFastTravel.
  __declspec(naked) void FastTravelBeginHook()
  {
    __asm {
      pushfd
      pushad
      mov eax, [ebp - 0x24]
      push dword ptr [eax + 0x1EC]
      call DispatchFastTravelBegin
      test al, al
      jz cancelled
      popad
      popfd
      mov ecx, [ebp - 0x24]
      mov edx, [ecx + 0x1EC] // replay nine displaced bytes
      push 0x93BEE1
      ret
    cancelled:
      popad
      popfd
      push 0x93C171 // native cleanup frees the request and clears its pointer
      ret
    }
  }

}

void OnFastTravelBegin::RegisterEvent(NVSEEventManagerInterface *eventInterface)
{
  nvse_event_manager = eventInterface;
  static NVSEEventManagerInterface::ParamType params[] = {
      NVSEEventManagerInterface::eParamType_Reference};
  nvse_event_manager->RegisterEvent(kFastTravelBeginEventName, 1, params, NVSEEventManagerInterface::kFlag_FlushOnLoad);
}
void OnFastTravelBegin::InstallHook()
{
  WriteRelJump(0x93BED8, reinterpret_cast<uintptr_t>(&FastTravelBeginHook));
}
