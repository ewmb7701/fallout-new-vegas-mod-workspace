[CmdletBinding()]
param()
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'common.ps1')
$mod = Get-ModContext
if ($mod.Kind -notin @('mod', 'fomod')) { throw 'Use build-nvse-plugin.ps1 for native plugins.' }
Reset-ModTarget $mod
Get-ChildItem -LiteralPath $mod.Source -Force | ForEach-Object {
    Copy-Item -LiteralPath $_.FullName -Destination $mod.Target -Recurse -Force
}
Write-Host "Built $($mod.Name): $($mod.Target)"
