[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateNotNullOrEmpty()]
    [ValidatePattern('^[A-Za-z0-9][A-Za-z0-9._-]*$')]
    [string]$Version
)

$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
$releaseDir = Join-Path $repoRoot "target\Release"
$pluginFile = Join-Path $releaseDir "ewmb_nvse.dll"
$distDir = Join-Path $repoRoot "dist"
$outputFile = Join-Path $distDir "ewmb_nvse_$Version.zip"

if (-not (Test-Path -LiteralPath $pluginFile -PathType Leaf)) {
    throw "Release DLL not found: '$pluginFile'. Run '.\scripts\build.ps1 -Configuration Release' first."
}

$tempRoot = [System.IO.Path]::GetFullPath([System.IO.Path]::GetTempPath()).TrimEnd('\', '/')
$stagingDir = Join-Path $tempRoot ("ewmb_nvse_package_" + [guid]::NewGuid().ToString("N"))
$pluginDir = Join-Path $stagingDir "NVSE\Plugins"

try {
    New-Item -ItemType Directory -Path $distDir -Force | Out-Null
    New-Item -ItemType Directory -Path $pluginDir -Force | Out-Null
    Copy-Item -LiteralPath $pluginFile -Destination $pluginDir

    Compress-Archive -LiteralPath (Join-Path $stagingDir "NVSE") -DestinationPath $outputFile -CompressionLevel Optimal -Force
}
finally {
    if (Test-Path -LiteralPath $stagingDir) {
        $resolvedStagingDir = [System.IO.Path]::GetFullPath($stagingDir)
        if (-not $resolvedStagingDir.StartsWith($tempRoot + [System.IO.Path]::DirectorySeparatorChar, [System.StringComparison]::OrdinalIgnoreCase)) {
            throw "Refusing to remove staging directory outside the temporary folder: '$resolvedStagingDir'."
        }
        Remove-Item -LiteralPath $stagingDir -Recurse -Force
    }
}

Write-Host "Created $outputFile" -ForegroundColor Green
