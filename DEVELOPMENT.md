# Development

Run these commands from the workspace root in the same PowerShell session.
First select a package with `source-<package_id>.ps1`, then build and package it.
The selector sets `MOD_*` environment variables consumed by the shared scripts.
Run another selector to switch packages. Do not run each step in a separate
PowerShell process, since the selected environment would be lost.

## Workspace layout

| Path                      | Contents                                                     |
| ------------------------- | ------------------------------------------------------------ |
| `source/<package_id>/`    | Editable mod files, FOMOD inputs, or native project sources  |
| `target/<package_id>/`    | Built files, including native DLLs and compiler/test outputs |
| `dist/<package name>.zip` | Distributable ZIP using the mod display name                 |
| `scripts/`                | Package selectors and shared build/package scripts           |
| `docs/<package_id>/`      | Documentation and publishing text                            |

Build scripts clear the selected package's target before rebuilding. `target/`
and `dist/` are ignored by Git. Paths are resolved from the workspace scripts,
independently of the shell's current directory.

## Regular mods

```powershell
./scripts/source-armor-repair-kits-ysi.ps1
./scripts/build-mod.ps1
./scripts/package-zip.ps1
```

`build-mod.ps1` copies the contents of `source/<package_id>/` into
`target/<package_id>/`. Packaging places those contents at the ZIP root, ready
for installation into the game's Data folder.

## NVSE plugins

Install Visual Studio with the Desktop development with C++ workload, the
v145 toolset, and a Windows SDK. The projects build for Win32.

```powershell
./scripts/source-ewmb-nvse.ps1
./scripts/build-nvse-plugin.ps1 -Configuration Release
./scripts/package-zip.ps1
```

`Release` is the default; use `-Configuration Debug` for a debug build.
The DLL is built directly into `target/<package_id>/`. Native ZIPs contain only
the DLL under `NVSE/Plugins/`; compiler intermediates, test executables, and
documentation are excluded. The existing runtime DLL filenames are preserved.

### Native tests

```powershell
./scripts/source-latest-character-save.ps1
./scripts/build-nvse-plugin.ps1 -Configuration Release -Test
./scripts/package-zip.ps1
```

`latest-character-save` includes its native integration test harness.
`fast-travel-pipboy` references a `tests.cpp` file absent from the imported
sources, so requesting `-Test` reports that missing harness. `ewmb-nvse` has
no native test target. Native tests do not replace in-game validation.

## FOMODs

```powershell
./scripts/source-jsawyerless.ps1
./scripts/build-fomod.ps1
./scripts/package-zip.ps1
```

`build-fomod.ps1` checks the file, folder, and image references in the source
`fomod/ModuleConfig.xml`, then copies the full installer source into the target.
`package-zip.ps1` creates the ZIP preserving that installer layout.

## Documentation

## Packaging behavior

Packaging uses the existing target and does not build automatically. Rebuild
after changing source files. Repackaging replaces `dist/<mod name>.zip` only
after the new archive has been created successfully.

These scripts build and package locally; they do not install into MO2 or the game.
