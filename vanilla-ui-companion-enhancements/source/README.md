# Vanilla UI Companion Enhancements

Activation-range companions show the stats panel. Outside activation range,
`ObjectUnderReticle` selects a companion for our standalone `CompanionHealthBar`
panel. Leaving a companion's activation target also selects this health panel.

CompanionHealthBar uses native enemy-health bracket artwork, centered whole pips,
and a name below the bar. It owns its target and timer: aiming refreshes the timer,
selection events refresh it, and looking away lets it expire
using `Behavior:fCompanionHealthBarTimer` from the INI (default: 5.5 seconds,
matching the vanilla enemy-health timer). It clears when the native activation prompt becomes visible,
hides during menus, and clears during VATS. Invalid, dead, disabled, or dismissed
companions are cleared. Native companion health-target proposals also select or refresh CompanionHealthBar,
including companions outside the reticle.

`OnEnemyHealthTargetChange` vetoes companion proposals and selects our health
panel instead. Stats targets are set and cleared only by the crosshair events. Non-companion actor
proposals are allowed and clear CompanionHealthBar. Null proposals leave our timer
and target intact. VATS proposals are allowed and clear/hide our panels.
A companion under the distant reticle directly selects or refreshes CompanionHealthBar,
including after an enemy replacement or leaving VATS. No previous-reticle state
or rebase flag gates selection. Companion native-target proposals are vetoed and select CompanionHealthBar directly.
`OnCompanionHit` selects CompanionHealthBar through `SetCompanionHealthBarTarget`.
The setter clears an existing native target, refreshes the prompt after clearing,
hides the native health tiles, selects the companion, and restarts the timer.
`UpdateCompanionHealthBar` handles validation, current HP, and companion visibility.
The resulting null-target event leaves our companion target and timer intact.
There is no deferred clear callback or pending-clear state.
The setter is used only to clear, never to assign a companion.

`Setup` registers crosshair events and the targeting loop. `SetCompanionStatsTarget`
manages activation stats and hands outgoing companions to `SetCompanionHealthBarTarget`.
`UpdateCompanionHealthBar` runs from the targeting loop. `Teardown` removes callbacks
and clears both panels. Activation picks take priority; object-type filtering
ignores scenery, and distant picks require a player-teammate NPC or creature.

## Install

Requires xNVSE 6.3+, JIP LN NVSE, JohnnyGuitar NVSE 4.55+, ShowOff NVSE 1.55+,
UIO 2.30+, and EWMB NVSE with `OnEnemyHealthTargetChange`.
Enable the shared core and one configuration package. Both panels use the installed
HUD textures; no custom texture package is required. The activation name and action
remain native and visible.
Choose **Config (Default)** for the default settings or **Config (ewmb)** for the personal preset. Both supply the same INI; enable exactly one. The core contains no configuration file.

No ESP/ESM, MCM, or reload command.

## Settings and layout

Edit `config/VanillaUICompanionEnhancements.ini` in your chosen configuration mod and restart the game.
The stats panel starts at the default activation-prompt position.
Set `Position:fStatsOffsetX` and `Position:fStatsOffsetY` to move the entire stats
panel, including its bracket. Both default to `0`; positive X moves right and
positive Y moves down. Match custom HUD Editor positions manually with these
settings. `Position:fHealthBarOffsetX` and `Position:fHealthBarOffsetY` move the standalone
companion health bar, bracket, and name together in HUD units. Both default to
`0`; positive X moves right and positive Y moves down.

HP appears beneath WG in the left column. Display modes are:

| Mode | `iHPStatDisplayMode` | `iWGStatDisplayMode` |
| --- | --- | --- |
| 0 | Hide HP label/value | Hide WG label/value |
| 1 | Current health (default) | Current weight (default) |
| 2 | Current / maximum health | Weight / capacity |
| 3 | Health percentage | Capacity-used percentage |
| 4 | Difference from maximum health, e.g. `HP -25` | Spare capacity: `+` free, `-` overweight |
| 5 | Damage taken (missing health), e.g. `DAM 25` | Falls back to mode 1 |

Hiding a stat leaves the other stats unchanged.
Invalid modes fall back to current values (mode 1). Both config packages have
been migrated to retain their previous display choices.
HP values and percentages round down. Missing health uses displayed maximum
minus displayed current HP, clamped to zero. Mode 4 displays the difference with
a minus sign (`HP -25`); mode 5 displays a positive damage amount (`DAM 25`).
Full health shows `HP -0` or `DAM 0`, respectively.
The pip meter uses the unrounded health ratio. Health percentage displays `--%`
if maximum health is nonpositive. Weight percentages can exceed 100%; nonpositive
capacity displays `WG --%`. Weight values round to whole numbers, and spare
capacity displays `0` at the limit. Labels are left-aligned and values right-aligned.

Set `Appearance:bShowDRStat=1` to add a separate DR percentage line beneath DT
(for example `DR 25%`). It defaults to `0` (hidden), and sits beside HP.

The stats panel follows the native activation prompt color. The standalone health
panel uses `Appearance:uHealthBarRGB` in Stewie-style `0xRRGGBB` hexadecimal format.
It defaults to companion blue (`0x2ECEFF`) and colors its bracket, pips, and name.

The standalone companion health bar uses a 240-unit full health meter, 8-unit pips, and engine center
X 171. Pips decrease in pairs, with one pip at positive health below the first
pair and none at zero. The numeric HP row uses the same health fraction.



## In-game verification

After restarting, check activation stats, distant companions, and leaving
activation range. Confirm that CompanionHealthBar updates with damage and expires
after looking away, yields to activation prompts, and hides during menus/VATS.
Companion native-health proposals must be vetoed; enemy proposals must replace
CompanionHealthBar. Check null proposals and VATS separately.
Static script/XML checks cannot confirm runtime event timing or rendering.

API references: [ObjectUnderReticle](https://geckwiki.com/index.php/ObjectUnderReticle),
[crosshair on](https://geckwiki.com/index.php/OnCrosshairOn),
[crosshair off](https://geckwiki.com/index.php?title=OnCrosshairOff).

WG occupies the upper-left row; HP occupies the lower-left row.
Set `Appearance:iHPStatDisplayMode=0` to hide the HP label and value without
hiding the health bar or moving the other stats.

Companion health expiry uses one restartable ShowOff auxiliary timer and
`OnCompanionHealthBarTimerExpired`. It pauses in menus and is removed on load. Clearing
the panel cancels the timer without firing its callback. Current HP is read by
`UpdateCompanionHealthBar`; no previous-HP value or per-frame countdown is stored.
`UpdateCompanionStats` refreshes the activation stats.

`Setup` registers one global JIP `SetOnHitEventHandler`; `Teardown` removes it.
`OnCompanionHit` filters the struck actor with `GetPlayerTeammate`, so party changes
need no registration updates or roster polling. Outside VATS, a hit selects the
companion health target and restarts the expiry timer. Hits do not change stats targets.
This responds to hits (even hits that cause no health loss), not poison, falling,
or scripted DamageAV. Current HP is read by the existing update callbacks.

Prompt arbitration follows the native HUD action/name visible traits, sampled in the
update loop after crosshair events. It does not infer activatability from object
type or combat state. Selecting a companion for activation clears the native enemy
health target and calls UpdateCrosshairPrompt.
