[CmdletBinding()]
param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Debug",

    [switch]$Install
)

$ErrorActionPreference = "Stop"

$workspaceRoot = Split-Path -Parent $PSScriptRoot
$project = Join-Path $workspaceRoot "source\ewmb_nvse.vcxproj"

$vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path -LiteralPath $vswhere)) {
    throw "Visual Studio Installer (vswhere.exe) was not found. Install Visual Studio Build Tools with Desktop development with C++."
}

$installationPath = & $vswhere -latest -products * -requires Microsoft.Component.MSBuild -property installationPath
if (-not $installationPath) {
    throw "No Visual Studio installation containing MSBuild was found. Install the Desktop development with C++ workload."
}

$msbuild = Join-Path $installationPath "MSBuild\Current\Bin\MSBuild.exe"
if (-not (Test-Path -LiteralPath $msbuild)) {
    throw "MSBuild was not found at '$msbuild'."
}

$arguments = @(
    $project,
    "-m",
    "-restore",
    "-property:Configuration=$Configuration",
    "-property:Platform=Win32",
    "-verbosity:minimal"
)

if ($Install) {
    if (-not $env:FalloutNVPath) {
        throw "FalloutNVPath is not set. Set it to the folder containing FalloutNV.exe, then restart VS Code."
    }
    if (-not (Test-Path -LiteralPath $env:FalloutNVPath -PathType Container)) {
        throw "FalloutNVPath does not exist: '$env:FalloutNVPath'."
    }
} else {
    $arguments += "-property:PostBuildEventUseInBuild=false"
}

Write-Host "Building ewmb_nvse ($Configuration|Win32) with $msbuild"
& $msbuild @arguments
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}
