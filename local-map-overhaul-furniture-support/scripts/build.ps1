[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'common.ps1')
if (!(Test-Path -LiteralPath $sourceDir -PathType Container)) {
    throw "Source directory not found: $sourceDir"
}
# Recreate the package's own staging folder so removed source files stay removed.
if (Test-Path -LiteralPath $targetDir) {
    $resolvedTarget = (Resolve-Path -LiteralPath $targetDir).Path
    $expectedTarget = [System.IO.Path]::GetFullPath((Join-Path $packageRoot 'target'))
    if ($resolvedTarget -ne $expectedTarget -or
        ((Get-Item -LiteralPath $targetDir -Force).Attributes -band [System.IO.FileAttributes]::ReparsePoint)) {
        throw "Refusing to clean an unexpected target directory: $targetDir"
    }
    Remove-Item -LiteralPath $resolvedTarget -Recurse -Force
}
New-Item -ItemType Directory -Path $targetDir | Out-Null
Get-ChildItem -LiteralPath $sourceDir -Force | ForEach-Object {
    Copy-Item -LiteralPath $_.FullName -Destination $targetDir -Recurse -Force
}
Write-Host "Built $modName into $targetDir"
