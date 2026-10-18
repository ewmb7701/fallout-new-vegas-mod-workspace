# ewmb NVSE

## Events

### OnWorldMapBuild

An event handler added by the ewmb NVSE plugin version 3.

#### Description

Runs after the engine finishes building the world map, once native map marker
creation and positioning have completed. Use it to update map tiles or apply
marker restrictions after each rebuild.

Handlers run synchronously before the code that requested the build resumes.

#### Dispatched Args

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
Use this event to update information or draw a path for the selected marker.

#### Dispatched Args

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
travel and `FastTravel`. `FastTravelEx` does not dispatch it. Other scripted
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

#### Syntax

```text
SetEventHandler "OnFastTravelBegin" handler:userDefinedFunction
```

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

#### See Also

- [SetPendingFastTravelMarker](#setpendingfasttravelmarker)
- [FastTravel](#fasttravel)
- [SetEventHandler](https://geckwiki.com/index.php?title=SetEventHandler)

### OnEnemyHealthTargetChange

An event handler added by the ewmb NVSE plugin version 5.

#### Description

Runs before the target of `HUDMainMenu/EnemyHealth` changes. The target is shared by the realtime
and VATS health displays and is separate from the player's combat target.
Handlers run synchronously and can prevent the change by returning `0` with
`SetFunctionValue`. `GetEnemyHealthTarget` returns the previous target while the
handlers run.

#### Dispatched Args

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
scn RedirectFastTravelToHooverDam

ref rMarker

Begin Function {rMarker}
    ; Redirect the pending trip to the Hoover Dam map marker.
    ; Return 0 if replacement fails, cancelling the original trip.
    SetFunctionValue (SetPendingFastTravelMarker WastelandNVVHDMapMarkerREF)
End
```

Register this handler to redirect marker travel to Hoover Dam:

```text
SetEventHandler "OnFastTravelBegin" RedirectFastTravelToHooverDam
```

`WastelandNVVHDMapMarkerREF` is the Hoover Dam map marker
(`FalloutNV.esm:00135E5F`). It must already be visible and travel-enabled.

#### Notes

- Return 1 from the event handler after successful replacement; return 0 on
  failure to prevent travel to the original marker.
- The marker must be visible and travel-enabled, not disabled or deleted.
  Path permissions remain the caller's responsibility.
- Call inside `OnFastTravelBegin` to redirect before the engine resolves the
  destination. The command itself is not event-scoped: it writes the current
  player request directly. Once the executor argument has been resolved, changing
  that request is too late to redirect travel. No UI tile lookup is involved.

#### See Also

- [CanPlayerFastTravelWithMessage](#canplayerfasttravelwithmessage)
- [OnFastTravelBegin](#onfasttravelbegin)
- [FastTravel](#fasttravel)
- [GetPath](#getpath)
- [GetExteriorCellEx](#getexteriorcellex)
- [IsCellVisitedEx](#iscellvisitedex)
- [IsCellExpiredEx](#iscellexpiredex)
- [FastTravelEx](#fasttravelex)

### GetExteriorCellEx

A function added by the ewmb NVSE plugin version 8.

#### Description

Returns the exterior CELL form at the specified worldspace and cell-grid coordinates.

Unlike [GetExteriorCell](https://geckwiki.com/index.php/GetExteriorCell), which
only looks in the worldspace's resident cell map, this function also attempts to
read and instantiate the CELL record from the worldspace's contributing plugin
files when no object is present in that map. A cell does not need to be attached
for gameplay to be returned by either function; the difference is whether its
CELL object already exists in the map.

Returns `0` for invalid arguments, a missing record, or a rejected or failed read.

#### Syntax

```text
(cell:form) GetExteriorCellEx worldspace:worldspace coordX:int coordY:int
```

No calling reference. Coordinates are **cell-grid indices**, matching
`GetExteriorCell`, not world positions.

#### Example

```text
ref rCell
int iVisited
int iExpired
let rCell := GetExteriorCellEx WastelandNV -22 -19
if rCell
    let iVisited := IsCellVisitedEx rCell
    let iExpired := IsCellExpiredEx rCell
endif
```

#### Notes

- This query can create an engine-owned CELL record and add it to the worldspace
  map. It is not a read-only state query.
- Does not intentionally load child references, attach the cell, move the player,
  create scenery, or mark the cell visited. Ordinary engine loading remains
  responsible for loading the cell's children later.
- Does not apply saved CELL changes. Use the `Ex` state functions to consult
  saved state when it is absent from the returned CELL object.
- Does not create empty cells where no plugin CELL record exists.
- Missing-cell resolution is rejected during
  save/load or child-record loading. Existing cells use the normal map lookup.
- Record lookup uses the engine's file lookup and available cell-offset tables,
  including those populated by Cell Offset Generator. That plugin is optional;
  missing offsets can make the lookup slower. Contributing files are processed
  in native order so their CELL overrides are considered.

#### See Also

- [GetExteriorCell](https://geckwiki.com/index.php/GetExteriorCell)
- [IsCellVisitedEx](#iscellvisitedex)
- [IsCellExpiredEx](#iscellexpiredex)

### IsCellVisitedEx

A function added by the ewmb NVSE plugin version 8.

#### Description

Returns `1` when the specified cell has live seen data or its engine change
record contains saved seen data. Returns `0` otherwise, including invalid input.

Unlike [IsCellVisited](https://geckwiki.com/index.php?title=IsCellVisited), which
checks the CELL object's live seen data, this function also checks the engine's
in-memory change-record collection when live seen data is absent. This allows
saved visitation to be read without attaching the cell or applying its changes.

#### Syntax

```text
(visited:int) IsCellVisitedEx cell:form
```

No calling reference. The argument must be a CELL form.

#### Example

```text
int iVisited
set iVisited to IsCellVisitedEx PrimmNashResidence
```

#### Notes

- Preserves the original function's meaning of visited: seen data is required.
  A cell loaded nearby, or one with only a detach timestamp, is not necessarily visited.
- Does not mark the cell visited or force the engine to update seen data.
- On a first visit, an early event such as `OnCellEnter` can still precede the
  seen-data update. The saved-record fallback does not remove that timing issue.
- Requires an existing CELL form; use `GetExteriorCellEx` to resolve exterior cell-grid coordinates.

#### See Also

- [IsCellVisited](https://geckwiki.com/index.php?title=IsCellVisited)
- [IsCellExpiredEx](#iscellexpiredex)
- [GetExteriorCellEx](#getexteriorcellex)

### IsCellExpiredEx

A function added by the ewmb NVSE plugin version 8.

#### Description

Checks whether the specified cell is due to reset, using its detach timestamp.

| Return | Meaning                                                                   |
| ------ | ------------------------------------------------------------------------- |
| `-1`   | Detach time is absent, unreadable, or zero; expiry cannot be established. |
| `0`    | The cell has not expired, or the argument is invalid.                     |
| `1`    | The reset interval has elapsed, or the timestamp is a reset sentinel.     |

Unlike [IsCellExpired](https://geckwiki.com/index.php?title=IsCellExpired), which
reads the CELL object's live detach data, this function can also read detach data
from the engine's in-memory change records. Live detach data takes precedence.
The saved fallback is used only when both live detach data and live seen data are
absent, to avoid using a stale saved timestamp for an active cell.

#### Syntax

```text
(expired:int) IsCellExpiredEx cell:form
```

No calling reference. The argument must be a CELL form.

#### Example

```text
int iExpired
set iExpired to IsCellExpiredEx PrimmNashResidence
if iExpired == 1
    ; The cell is due to reset.
endif
```

#### Notes

- Uses the original timestamp rules: reset sentinel timestamps `-1` and `-2`
  produce `1`; otherwise compares `floor(GameDaysPassed * 24) - detachTime`
  against `iHoursToRespawnCell`.
- The default reset interval is 72 game hours; the game setting controls it.
- The original function documents `-1` for an unvisited or currently occupied
  cell. This extension also returns `-1` if the fallback timestamp cannot be decoded.
- Expiry does not establish visitation. Query `IsCellVisitedEx` separately.
- Does not load or attach the cell, apply change records, or trigger a reset.

#### See Also

- [IsCellExpired](https://geckwiki.com/index.php?title=IsCellExpired)
- [IsCellVisitedEx](#iscellvisitedex)
- [GetExteriorCellEx](#getexteriorcellex)

### GetPath

A function added by the ewmb NVSE plugin version 8.

#### Description

Returns the native path between two references as a stringmap. Returns an
uninitialized array on invalid input or native path-search failure.

| Key        | Type  | Value                                     |
| ---------- | ----- | ----------------------------------------- |
| `distance` | float | Native path distance in game units.       |
| `points`   | array | Ordered packed array of point stringmaps. |

Each point contains `reference`, `worldspace`, `cell`, `x`, `y`, `z`, and `kind`.
The sequence is an `origin` point, zero or more `exit`/`arrival` pairs, and a
`destination` point. Exterior points may have a null cell; interior points have
no worldspace. A valid zero-distance path still has origin and destination points.

#### Syntax

```text
(path:array) GetPath originRef:reference destinationRef:reference
```

No calling reference. Both arguments are object references.

#### Example

```text
; rLanding is the desired destination reference.
array_var aPath
let aPath := GetPath Player rLanding
if eval aPath != Ar_Null
    array_var aPoints = aPath["points"]
    float fDistance = aPath["distance"]
endif
```

#### Notes

- Returns the engine's coarse cell/worldspace and teleport-door path, not
  intermediate map markers or detailed navigation around terrain obstacles.
- An exit identifies a teleport door. Its arrival identifies the linked door
  and uses teleport landing coordinates, which may differ from that door's position.
- Endpoints are used as supplied. Resolve a marker's linked landing reference
  first when matching its fast-travel destination.
- Uses the player's door-access context. Does not move references, activate doors,
  advance time, or change discovery flags.
- Points are copied before temporary native search storage is released.
- Does not calculate travel time. If calculating time from adjacent points,
  exclude exit-to-arrival teleport gaps from the travelled distance.

#### See Also

- [FastTravel](#fasttravel)
- [FastTravelEx](#fasttravelex)
- [GetTeleportDoor](https://geckwiki.com/index.php?title=GetTeleportDoor)

### CanPlayerFastTravelWithMessage

A function added by the ewmb NVSE plugin version 8.

#### Description

Runs the native player fast-travel eligibility check, with rejection messages
enabled. Returns `1` when permitted, otherwise `0`. Does not initiate travel.

#### Syntax

```text
(allowed:bool) CanPlayerFastTravelWithMessage
```

No calling reference or arguments.

#### Example

```text
if CanPlayerFastTravelWithMessage
    FastTravelEx rSpace fX fY fZ 0 0 fHeading fHours
endif
```

#### Notes

- Uses the current map-menu eligibility helper, including other plugins' hooks.
- Applies the native departure restrictions, including restrictions on interior
  departures. It does not check the supplied destination or path.
- Returns `0` if the player or expected helper call is unavailable; those technical
  failures do not display a gameplay rejection message.
- `FastTravel` already performs this check; `FastTravelEx` does not.

#### See Also

- [FastTravel](#fasttravel)
- [FastTravelEx](#fasttravelex)

### FastTravelEx

A function added by the ewmb NVSE plugin version 8.

#### Description

Travels to coordinates with an explicit elapsed duration. Uses native-style
loading preparation, time and actor updates, player recovery, follower movement,
and post-travel cleanup. Returns `1` after execution, or `0` when rejected.

#### Syntax

```text
(success:bool) FastTravelEx space:form x:float y:float z:float rotX:float rotY:float rotZ:float hours:float
```

No calling reference. All arguments are required.

| Argument               | Description                                                       |
| ---------------------- | ----------------------------------------------------------------- |
| `space`                | Worldspace for exterior travel, or CELL form for interior travel. |
| `x`, `y`, `z`          | Destination position in game units.                               |
| `rotX`, `rotY`, `rotZ` | Destination rotation in degrees.                                  |
| `hours`                | Nonnegative elapsed game hours; fractional values are retained.   |

#### Example

```text
; Destination variables come from the caller's path or location data.
; Loading may run script callbacks before this call returns.
if eval (FastTravelEx rSpace fX fY fZ 0 0 fHeading fHours) == 0
    PrintC "Unable to travel to the destination."
endif
```

#### Notes

- Exterior CELL arguments are rejected. Supply a worldspace; the engine resolves
  and loads the exterior cell from the position.
- Rejects nonfinite coordinates/rotations/time, negative or overflowing hours,
  invalid exterior coordinates, and nonpositive or nonfinite TimeScale.
- Rejects missing engine state, an existing position request, or reentrant
  coordinate travel. These are execution guards, not gameplay eligibility checks.
- Executes synchronously, but loading can run callbacks. A successful return
  does not replace checking the player's actual destination when arrival matters.
- Preserves fractional hours even when the total exceeds one hour. Respects
  native travel autosave settings and throttling.
- Does not call `CanPlayerFastTravelWithMessage` or perform the full native
  fast-travel eligibility check. Its own rejection checks return `0` without
  displaying the native rejection message. Unlike `FastTravel`, it leaves full
  eligibility checking and any messages to the caller. Call
  `CanPlayerFastTravelWithMessage` when native eligibility rules are desired.
  Without that check, interior departures are allowed.
- Does not calculate or validate a path. Destination permissions are the caller's
  responsibility.
- For markers, use linked landing coordinates where present. For doors, supply
  teleport landing coordinates and rotation; this command does not activate the door.
- Eligible followers are identified before relocation and moved to the player afterward.
- Does not dispatch `OnFastTravelBegin` or JIP/ITR marker-travel events. Patches
  attached solely to marker travel are not invoked. The installed low-process
  actor-update helper is used, retaining fixes applied to that helper.
- Uses a separate coordinate implementation without patching marker travel or
  creating temporary markers. If replacing a native request, cancel it first
  and call this command on a later update, after request cleanup.

#### See Also

- [CanPlayerFastTravelWithMessage](#canplayerfasttravelwithmessage)
- [FastTravel](#fasttravel)
- [GetPath](#getpath)
- [OnFastTravelBegin](#onfasttravelbegin)

### FastTravel

A function added by the ewmb NVSE plugin version 5.

#### Description

Submits a full native fast-travel request for a map marker, without a confirmation
or any explicit Pip-Boy open/close action. Checks native player eligibility with
rejection messages enabled and checks marker availability.

#### Syntax

```text
(queued:bool) FastTravel mapMarker:reference
```

No calling reference is required. Returns `1` when the native request is submitted,
or `0` on failure. Submission does not report eventual arrival or a later event veto.

#### Example

```text
if eval (FastTravel WastelandNVVHDMapMarkerREF) == 0
    PrintC "The fast-travel request could not be submitted."
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
  run synchronously and execute script callbacks before returning.
- Resolves the marker by reference without tiles or simulated clicks.
- Does not change discovery flags. Callers must enforce their own path permissions;
  restrictions confined to marker tiles or click handlers are bypassed.

#### See Also

- [CanPlayerFastTravelWithMessage](#canplayerfasttravelwithmessage)
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
