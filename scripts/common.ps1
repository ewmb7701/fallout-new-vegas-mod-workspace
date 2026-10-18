function Get-ModContext {
    $root = Split-Path -Parent $PSScriptRoot
    if ($env:MOD_WORKSPACE -ne $root -or $env:MOD_ID -notmatch '^[a-z0-9]+(-[a-z0-9]+)*$') {
        throw 'Run scripts/source-<package>.ps1 first in this PowerShell session.'
    }
    if (!$env:MOD_NAME -or $env:MOD_NAME.IndexOfAny([IO.Path]::GetInvalidFileNameChars()) -ge 0) {
        throw 'The selected mod name is not a valid ZIP filename.'
    }
    $source = Join-Path $root "source\$env:MOD_ID"
    if (!(Test-Path -LiteralPath $source -PathType Container)) { throw "Missing source: $source" }
    [pscustomobject]@{
        Root = $root; Id = $env:MOD_ID; Name = $env:MOD_NAME; Kind = $env:MOD_KIND
        Source = $source
        Target = Join-Path $root "target\$env:MOD_ID"
        Dist = Join-Path $root 'dist'
        Docs = Join-Path $root "docs\$env:MOD_ID"
        Project = $env:MOD_PROJECT; Dll = $env:MOD_DLL; TestProperty = $env:MOD_TEST_PROPERTY
    }
}

function Reset-ModTarget($Mod) {
    $expected = [IO.Path]::GetFullPath((Join-Path $Mod.Root "target\$($Mod.Id)"))
    if ([IO.Path]::GetFullPath($Mod.Target) -ne $expected) { throw 'Unexpected target path.' }
    # Never follow redirected output folders during cleanup.
    foreach ($path in @((Join-Path $Mod.Root 'target'), $Mod.Target)) {
        if ((Test-Path -LiteralPath $path) -and
            ((Get-Item -LiteralPath $path -Force).Attributes -band [IO.FileAttributes]::ReparsePoint)) {
            throw "Refusing redirected target: $path"
        }
    }
    if (Test-Path -LiteralPath $Mod.Target) {
        if ((Resolve-Path -LiteralPath $Mod.Target).Path -ne $expected) { throw 'Unexpected resolved target.' }
        Remove-Item -LiteralPath $expected -Recurse -Force
    }
    New-Item -ItemType Directory -Path $Mod.Target -Force | Out-Null
}
