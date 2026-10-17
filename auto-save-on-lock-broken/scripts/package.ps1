[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'common.ps1')
$outputFile = Join-Path $distDir "$modName.zip"

& (Join-Path $PSScriptRoot 'build.ps1')
New-Item -ItemType Directory -Path $distDir -Force | Out-Null
Add-Type -AssemblyName System.IO.Compression.FileSystem
# Keep the previous ZIP until the new archive has been created successfully.
$tempFile = Join-Path $distDir ("$modName." + [guid]::NewGuid().ToString('N') + '.tmp')
try {
    [System.IO.Compression.ZipFile]::CreateFromDirectory(
        $targetDir, $tempFile, [System.IO.Compression.CompressionLevel]::Optimal, $false)
    Move-Item -LiteralPath $tempFile -Destination $outputFile -Force
} finally {
    if (Test-Path -LiteralPath $tempFile) {
        Remove-Item -LiteralPath $tempFile -Force
    }
}
Write-Host "Created $outputFile"
