# ewmb NVSE

Documentation for ewmb NVSE v5.

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

## Functions

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
