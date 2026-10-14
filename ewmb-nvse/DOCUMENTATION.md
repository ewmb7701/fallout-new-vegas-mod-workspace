# ewmb NVSE

## Events

### OnWorldMapBuild

An event handler added by the ewmb NVSE plugin version 3.

#### Description

Runs after the engine finishes building the world map, once native map marker
creation and positioning have completed. Use it to update map tiles or apply
marker restrictions after each rebuild.

Handlers run synchronously before the code that requested the build resumes.

##### Dispatched Args

No arguments are dispatched to handlers. There is no calling reference.

#### Syntax

Use `SetEventHandler` / `SetEventHandlerAlt` with the event name `"OnWorldMapBuild"`.

#### Handler Script

A skeleton handler script for this event:

```text
scn MyWorldMapBuildHandler

Begin Function {}
    ; Update the completed world map here.
End
```

#### Example

```text
SetEventHandler "OnWorldMapBuild" MyWorldMapBuildHandler
```

To remove the handler:

```text
RemoveEventHandler "OnWorldMapBuild" MyWorldMapBuildHandler
```

#### Notes

- Register the handler again after each save load. Registered handlers are cleared when loading a save (`kFlag_FlushOnLoad`).
- This event runs on world-map builds, so it can fire again while the Pip-Boy remains open.
- Handler return values are ignored.

#### See Also

- [SetEventHandler](https://geckwiki.com/index.php?title=SetEventHandler)
- [RemoveEventHandler](https://geckwiki.com/index.php?title=RemoveEventHandler)

### OnWorldMapMarkerSelectionChange

An event handler added by the ewmb NVSE plugin version 4.

#### Description

Runs when the selected world-map marker changes or an existing selection is
cleared. Handlers run synchronously after the native selection field is updated.
Use this event to update information or draw a route for the selected marker.

##### Dispatched Args

There is no calling reference. The handler receives:

1. Ref: the newly selected map marker (`rSelectedMarker`), or `0` when selection clears.

#### Syntax

Use `SetEventHandler` / `SetEventHandlerAlt` with the event name
`"OnWorldMapMarkerSelectionChange"`.

#### Handler Script

```text
scn MyWorldMapMarkerSelectionChangeHandler

Begin Function {ref rSelectedMarker}
    if rSelectedMarker
        ; Update the display for this marker.
    else
        ; Clear the previous marker's display.
    endif
End
```

#### Example

Register a UDF:

```text
SetEventHandler "OnWorldMapMarkerSelectionChange" MyWorldMapMarkerSelectionChangeHandler
```

Or register a lambda:

```text
SetEventHandler "OnWorldMapMarkerSelectionChange" (begin function {ref rSelectedMarker}
    if rSelectedMarker
        printc "Selected map marker: %n" rSelectedMarker
    else
        printc "Map marker selection cleared"
    endif
end)
```

Remove the UDF handler:

```text
RemoveEventHandler "OnWorldMapMarkerSelectionChange" MyWorldMapMarkerSelectionChangeHandler
```

#### Notes

- Handlers are cleared on save load. Register them again after loading a save.
- An unchanged selection does not repeatedly dispatch the event. Leaving a marker and selecting it again produces a new selection change.
- The argument is the marker reference, not its UI tile or a numeric form ID. Read marker properties directly from the reference.
- Only world-map selections dispatch this event. Map closure and tab changes are not separate triggers for it.
- Handler return values are ignored.

#### See Also

- [OnWorldMapBuild](#onworldmapbuild)
- [SetEventHandler](https://geckwiki.com/index.php?title=SetEventHandler)
- [SetEventHandlerAlt](https://geckwiki.com/index.php?title=SetEventHandlerAlt)
- [RemoveEventHandler](https://geckwiki.com/index.php?title=RemoveEventHandler)

### OnFastTravelBegin

An event handler added by the ewmb NVSE plugin version 5.

#### Description

Runs in the player's position-request processor before it resolves the marker's
linked landing reference and before JIP's travel notification. Covers map-menu
travel and `FastTravelToMapMarker`, including automatic route legs. Other scripted
travel is covered when it uses the same native marker request path; direct
teleports or calls bypassing that path are not covered.

#### Timing compared with other events

All three events run before actual travel, not after loading or arrival. In the
inspected native request path, the order is:

| Event                    | Timing and destination                                                                                                                                                                                                                       |
| ------------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| EWMB `OnFastTravelBegin` | Before the request marker is resolved to its linked landing reference. Receives the original marker; `SetPendingFastTravelMarker` can redirect this request. Returning 0 cancels it before JIP's notification.                               |
| JIP `OnFastTravel`       | At the executor call site, after destination resolution. Receives the resolved destination, which may be a linked landing reference. Changing the pending marker at this point cannot change the argument already selected for the executor. |
| ITR `OnPreFastTravel`    | At the executor entry, after JIP's notification. Can veto execution, but JIP's notification has already happened. Its destination argument has already been resolved.                                                                        |

For map-menu travel, this follows accepting the confirmation and the native
Pip-Boy close flow. Directly submitted marker requests also dispatch
`OnFastTravelBegin`; no confirmation is required.

References: [JIP registration API](https://geckwiki.com/index.php?title=SetOnFastTravelEventHandler),
[ITR event](https://geckwiki.com/index.php?title=ITR:OnPreFastTravel), and
[ITR executor hook implementation](https://github.com/Into-The-Rough/itr-nvse/blob/master/itr-nvse/handlers/OnPreFastTravelHandler.cpp).

#### Dispatched Args

One reference: the request's original map marker. There is no calling reference.

#### Handler Script

```text
Begin Function {ref rMarker}
    SetFunctionValue 1
    ; SetPendingFastTravelMarker may replace the request destination here.
    ; Return 0 to veto execution.
End
```

#### Example

```text
SetEventHandler "OnFastTravelBegin" MyFastTravelBeginHandler
RemoveEventHandler "OnFastTravelBegin" MyFastTravelBeginHandler
```

#### Notes

- Any numeric handler result of 0 vetoes execution. Later handlers cannot undo
  it. Native cleanup frees the request and clears the player's request pointer;
  the executor and JIP travel notification are skipped.
- The processor reads the updated request marker after handlers finish.
- No on a confirmation creates no request and does not dispatch this event.
- Non-marker position requests do not dispatch this event.
- Do not submit another request inside the handler; redirect the current one.
- Handlers are cleared on save load; register them again afterward.
- Map-menu Pip-Boy closing still happens through the normal path.

### OnEnemyHealthTargetChange

An event handler added by the ewmb NVSE plugin version 5.

#### Description

Runs before the target of `HUDMainMenu/EnemyHealth` changes. The target is shared by the realtime
and VATS health displays and is separate from the player's combat target.
Handlers run synchronously and can prevent the change by returning `0` with
`SetFunctionValue`. `GetEnemyHealthTarget` returns the previous target while the
handlers run.

##### Dispatched Args

There is no calling reference. The handler receives:

1. Ref: the proposed actor reference (`rTarget`), or `0` when clearing the target.

#### Syntax

```text
SetEventHandler "OnEnemyHealthTargetChange" handler:userDefinedFunction
```

#### Handler Script

```text
scn MyEnemyHealthTargetChangeHandler

ref rTarget

Begin Function {rTarget}
    SetFunctionValue 1
    ; Return 0 to prevent this target change.
End
```

#### Example

Prevent player teammates from becoming the health-bar target, while allowing
other actors and target clearing:

```text
scn HideTeammateHealthBarHandler

ref rTarget

Begin Function {rTarget}
    SetFunctionValue 1
    if rTarget
        if rTarget.GetPlayerTeammate
            SetFunctionValue 0
        endif
    endif
End
```

Register the handler:

```text
SetEventHandler "OnEnemyHealthTargetChange" HideTeammateHealthBarHandler
```

Remove the handler:

```text
RemoveEventHandler "OnEnemyHealthTargetChange" HideTeammateHealthBarHandler
```

#### Notes

- The name matches the native `<rect name="EnemyHealth">` in `hud_main_menu.xml`
  (`HUDMainMenu/EnemyHealth`). Despite its name, this element can display friendly
  actors, including companions; it is not restricted to enemies.

- A numeric return value of `0` vetoes the change and retains the previous
  target. All handlers run; a later result cannot undo an earlier veto.
- Handlers are cleared when loading a save. Register them again after loading.
- Covers native target selection, including damage and VATS, and changes made
  through `SetEnemyHealthTarget`.
- Assigning the same target does not dispatch the event. Repeated attempts to
  select a vetoed target can dispatch repeatedly.
- Constructor/load initialization and other plugins writing the target field
  directly do not dispatch this event.
- A rejected injured actor is not drawn as the health-bar target. VATS hides
  the bar if a veto leaves no target.
- The event does not change combat targets, hostility, or the activation
  reference. Preventing a target change does not guarantee the activation
  prompt will appear; other native visibility rules still apply.
- Use `SetFunctionValue` to veto a change. Calling `SetEnemyHealthTarget` from
  inside the handler can dispatch the event again.

#### See Also

- [GetEnemyHealthTarget](#getenemyhealthtarget)
- [SetEnemyHealthTarget](#setenemyhealthtarget)
- [SetEventHandler](https://geckwiki.com/index.php?title=SetEventHandler)
- [RemoveEventHandler](https://geckwiki.com/index.php?title=RemoveEventHandler)
- [SetFunctionValue](https://geckwiki.com/index.php?title=SetFunctionValue)

## Functions

### GetEnemyHealthTarget

A function added by the ewmb NVSE plugin version 5.

#### Description

Returns the actor reference currently stored as the `EnemyHealth` target, or `0`
if there is none. This target is shared by the realtime and VATS health displays.

#### Syntax

```text
(target:ref) GetEnemyHealthTarget
```

No calling reference or arguments.

#### Example

```text
ref rTarget

let rTarget := GetEnemyHealthTarget
if rTarget
    PrintC "Health-bar target: %n" rTarget
endif
```

#### Notes

- The name matches the native `<rect name="EnemyHealth">` in `hud_main_menu.xml`
  (`HUDMainMenu/EnemyHealth`). Despite its name, this element can display friendly
  actors, including companions; it is not restricted to enemies.

- The returned reference identifies the stored target, not whether its health
  bar is currently visible. Visibility and display timers are handled separately.
- This is separate from the combat target returned by `GetCombatTarget`.
- During `OnEnemyHealthTargetChange`, this function returns the previous target.
  The proposed target is passed as the event's argument.

#### See Also

- [SetEnemyHealthTarget](#setenemyhealthtarget)
- [OnEnemyHealthTargetChange](#onenemyhealthtargetchange)
- [GetCombatTarget](https://geckwiki.com/index.php?title=GetCombatTarget)

### SetEnemyHealthTarget

A function added by the ewmb NVSE plugin version 5.

#### Description

Sets the `EnemyHealth` target to the specified actor. Omit the argument to clear
the target. Returns `1` on success, or `0` for an invalid argument, an unavailable
HUD, or a change vetoed by an `OnEnemyHealthTargetChange` handler.

#### Syntax

```text
(success:bool) SetEnemyHealthTarget actor:ref{Optional}
```

No calling reference. The optional argument must be an actor reference.

#### Example

Set an actor as the health-bar target:

```text
; rActor is an actor reference selected by the script.
SetEnemyHealthTarget rActor
```

Clear the target and refresh the activation prompt after a successful clear:

```text
if SetEnemyHealthTarget
    UpdateCrosshairPrompt
endif
```

#### Notes

- The name matches the native `<rect name="EnemyHealth">` in `hud_main_menu.xml`
  (`HUDMainMenu/EnemyHealth`). Despite its name, this element can display friendly
  actors, including companions; it is not restricted to enemies.

- Changes dispatch `OnEnemyHealthTargetChange` before updating the target. A veto
  leaves the previous target intact. Setting the same target does not dispatch
  another change event.
- Setting an actor refreshes the displayed name and display timer. Native
  visibility rules still apply.
- Clearing the target does not substitute the current VATS target.
- Does not change the player's combat target or the activation reference.
- The activation prompt checks the health-bar target when deciding whether to
  hide itself. This command does not refresh that prompt; use JohnnyGuitar NVSE's
  `UpdateCrosshairPrompt` after a successful change when needed.

#### See Also

- [GetEnemyHealthTarget](#getenemyhealthtarget)
- [OnEnemyHealthTargetChange](#onenemyhealthtargetchange)
- [UpdateCrosshairPrompt](https://geckwiki.com/index.php?title=UpdateCrosshairPrompt)
- [GetCombatTarget](https://geckwiki.com/index.php?title=GetCombatTarget)

### SetPendingFastTravelMarker

A function added by the ewmb NVSE plugin version 5.

#### Description

Replaces the marker in the player's current native position request. Works for
map-menu and directly submitted marker travel; does not depend on event dispatch.
This changes the request, not the map menu, and does not submit another trip.

#### Syntax

```text
(success:bool) SetPendingFastTravelMarker mapMarker:reference
```

No calling reference. Returns 1 on replacement, or 0 without replacement when
there is no marker travel request, the marker is unavailable, or native player
eligibility fails.
Eligibility rejection messages remain enabled.

#### Example

```text
Begin Function {ref rMarker}
    ; rFirstStop is supplied by the caller's route logic.
    SetFunctionValue (SetPendingFastTravelMarker rFirstStop)
End
```

#### Notes

- Return 1 from the event handler after successful replacement; return 0 on
  failure to prevent travel to the original marker.
- The marker must be visible and travel-enabled, not disabled or deleted.
  Route permissions remain the caller's responsibility.
- Call inside `OnFastTravelBegin` to redirect before the engine resolves the
  destination. The command itself is not event-scoped: it writes the current
  player request directly. Once the executor argument has been resolved, changing
  that request is too late to redirect travel. No UI tile lookup is involved.

#### See Also

- [OnFastTravelBegin](#onfasttravelbegin)
- [FastTravelToMapMarker](#fasttraveltomapmarker)

### FastTravelToMapMarker

A fucntion added by the ewmb NVSE plugin version 5.

#### Description

Submits a full native fast-travel request for a map marker, without a confirmation
or any explicit Pip-Boy open/close action. Checks native player eligibility with
rejection messages enabled and checks marker availability.

#### Syntax

```text
(queued:bool) FastTravelToMapMarker mapMarker:reference
```

No calling reference is required. Returns `1` when the native request is submitted,
or `0` on failure. Submission does not report eventual arrival or a later event veto.

#### Example

```text
if eval (FastTravelToMapMarker rNextStop) == 0
    ; Clear the active route.
endif
```

#### Notes

- Works with the Pip-Boy open or closed. To redirect an initial native
  map trip while preserving its native continuation, use
  `SetPendingFastTravelMarker` inside `OnFastTravelBegin` instead.
- Uses the engine's marker travel request, including linked landing references,
  elapsed time and normal travel notifications. This is not a `MoveTo` operation.
- The marker must be visible and travel-enabled, and cannot be disabled or deleted.
- Refuses to replace an existing player position request. Native submission may
  run synchronously; arrange script continuation state before calling.
- Resolves the marker by reference without tiles or simulated clicks.
- Does not change discovery flags. Callers must enforce their own route permissions;
  restrictions confined to marker tiles or click handlers are bypassed.

#### See Also

- [OnFastTravelBegin](#onfasttravelbegin)

### SetBaseActorValue

A function added by the ewmb NVSE plugin version 3.

#### Description

Sets an actor value on an actor base form. If no base form is supplied, uses
the calling actor reference's base form.

Returns 1 when the arguments and target are accepted and the engine's actor
base setter is called, or 0 for invalid arguments or a missing/invalid target.

#### Syntax

```text
(success:bool) [actorRef:reference].SetBaseActorValue actorValue:actorValue value:float actorBase:baseForm{Optional}
```

Or:

```text
(success:bool) [actorRef:reference].SetBaseAV actorValue:actorValue value:float actorBase:baseForm{Optional}
```

`actorBase` is optional. If omitted, an actor calling reference is required.
If supplied, it must be an NPC or creature base form and takes precedence
over the calling reference.

#### Example

```text
set iSuccess to Player.SetBaseActorValue Strength 7
```

Sets Strength on the player's base form. The same call using the alias:

```text
set iSuccess to Player.SetBaseAV Strength 7
```

To supply an explicit base form:

```text
set iSuccess to SetBaseActorValue Strength 7 rActorBase
```

`iSuccess` is a numeric variable and `rActorBase` is a ref variable containing
an NPC or creature base form.

#### Notes

- The target is the shared actor base record, so the edit is not scoped to just the calling reference.
- The command rejects resolved actor value IDs outside 0-76 and non-finite values.
- A return value of 1 does not verify the resulting value. Storage and any value-specific limits are handled by the engine.

- Opcode: `0x4318`. Alias: `SetBaseAV`.

#### See Also

- [GetBaseActorValue](https://geckwiki.com/index.php?title=GetBaseActorValue)
- [SetActorValue](https://geckwiki.com/index.php?title=SetActorValue)
