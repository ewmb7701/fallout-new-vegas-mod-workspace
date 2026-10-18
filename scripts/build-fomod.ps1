[CmdletBinding()]
param()
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'common.ps1')
$mod = Get-ModContext
if ($mod.Kind -ne 'fomod') { throw 'Select a FOMOD package first.' }
$configPath = Join-Path $mod.Source 'fomod\ModuleConfig.xml'
if (!(Test-Path -LiteralPath $configPath)) { throw "Missing FOMOD configuration: $configPath" }
[xml]$config = Get-Content -LiteralPath $configPath -Raw
foreach ($node in $config.SelectNodes('//file[@source] | //folder[@source] | //image[@path]')) {
    $relative = if ($node.Name -eq 'image') { $node.GetAttribute('path') } else { $node.GetAttribute('source') }
    if (!$relative) { continue }
    $path = [IO.Path]::GetFullPath((Join-Path $mod.Source $relative))
    if (!$path.StartsWith($mod.Source + '\', [StringComparison]::OrdinalIgnoreCase) -or
        !(Test-Path -LiteralPath $path)) { throw "Missing or invalid FOMOD asset: $relative" }
}
& (Join-Path $PSScriptRoot 'build-mod.ps1')
