# Shared package settings for build.ps1 and package.ps1.
$modName = 'Save Points Integration (Typewriters)'
$packageRoot = Split-Path -Parent $PSScriptRoot
$sourceDir = Join-Path $packageRoot 'source'
$targetDir = Join-Path $packageRoot 'target'
$distDir = Join-Path $packageRoot 'dist'
