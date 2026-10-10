# ewmb NVSE

Documentation for ewmb NVSE v4.

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

1. Reference: the newly selected map marker (`rSelectedMarker`), or `0` when selection clears.

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

## Functions

### SetBaseActorValue

A function added by the ewmb NVSE plugin version 3.

#### Description

Sets an actor value on an actor base form. If no base form is supplied, uses
the calling actor reference's base form.

Returns 1 when the arguments and target are accepted and the engine's actor
base setter is called, or 0 for invalid arguments or a missing/invalid target.

#### Syntax

```text
(success:0/1) [actorRef:reference].SetBaseActorValue actorValue:actorValue value:float actorBase:baseForm
```

Or:

```text
(success:0/1) [actorRef:reference].SetBaseAV actorValue:actorValue value:float actorBase:baseForm
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
