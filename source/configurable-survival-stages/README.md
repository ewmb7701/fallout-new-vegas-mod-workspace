# Configurable Survival Stages

Requires xNVSE, JIP LN NVSE (script runner and INI functions), and JohnnyGuitar NVSE (EditorID lookup).

Put INI files in `Data/Config/ConfigurableSurvivalStages/` (or that path inside an MO2 mod). The `gr_` script applies them when loading a save or starting a new game. Restart the game after removing a preset to restore plugin-defined values.

Files run in alphabetical filename order. Later files win for the same stage and property. Omitted entries leave the current value alone; the framework alone changes nothing.

```ini
[Thresholds]
Water1=400

[Effects]
Water1=Dehydration200
```

Keys are stage EditorIDs. Thresholds are positive integers. Effects are EditorIDs of existing SPEL (Actor Effect) records; custom effects require the plugin that defines them. Use separate comment lines beginning with `;`, without inline comments or quotes around values.

All five stages in each family are supported, including the final stage:

| Family | Stage EditorIDs |
| --- | --- |
| Dehydration | Water1 through Water5 |
| Starvation | hunger1 through hunger5 |
| Sleep deprivation | sleep1 through sleep5 |
| Radiation | Rad1 through Rad5 |

Other loaded stage records of these four types can also be targeted. Keep each family's thresholds increasing. Invalid stage/effect forms and nonpositive or fractional thresholds are silently skipped.

This configures stage thresholds and effect assignments. It does not edit the contents of an effect, need accumulation rates, or the separate HUD/text threshold game settings.

The companion `JSawyerless - Balance - Survival Stage Thresholds` mod supplies a complete preset extracted from the installed original `jsawyer.esp`, with unchanged final stages taken from `FalloutNV.esm`. It includes stage thresholds and matching HUD/text game settings, and requires Configurable Survival Stages and Stewie's Tweaks. It does not require JSawyer itself.

Function documentation: [SetHardcoreStageThreshold](https://geckwiki.com/index.php/SetHardcoreStageThreshold), [SetHardcoreStageEffect](https://geckwiki.com/index.php?title=SetHardcoreStageEffect), [GetINISection](https://geckwiki.com/index.php/GetINISection).
