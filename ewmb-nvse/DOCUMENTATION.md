# ewmb NVSE

Documentation for ewmb NVSE v3.

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

Use `SetEventHandler` with the event name `"OnWorldMapBuild"`.

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
