[CmdletBinding()]
param()
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'common.ps1')
$mod = Get-ModContext
if (!(Test-Path -LiteralPath $mod.Target -PathType Container) -or
    !(Get-ChildItem -LiteralPath $mod.Target -File -Recurse -Force | Select-Object -First 1)) {
    throw 'Build the selected package before packaging.'
}
if ($mod.Kind -eq 'nvse-plugin') {
    $dll = Join-Path $mod.Target $mod.Dll
    if (!(Test-Path -LiteralPath $dll -PathType Leaf)) { throw "Missing plugin DLL: $dll" }
}
New-Item -ItemType Directory -Path $mod.Dist -Force | Out-Null
Add-Type -AssemblyName System.IO.Compression.FileSystem
Add-Type -AssemblyName System.IO.Compression
$output = Join-Path $mod.Dist "$($mod.Name).zip"
$temporary = Join-Path $mod.Dist ([guid]::NewGuid().ToString('N') + '.tmp')
try {
    if ($mod.Kind -eq 'nvse-plugin') {
        $archive = [IO.Compression.ZipFile]::Open($temporary, [IO.Compression.ZipArchiveMode]::Create)
        try {
            [IO.Compression.ZipFileExtensions]::CreateEntryFromFile(
                $archive, $dll, "NVSE/Plugins/$($mod.Dll)", [IO.Compression.CompressionLevel]::Optimal) | Out-Null
        } finally { $archive.Dispose() }
    } else {
        [IO.Compression.ZipFile]::CreateFromDirectory(
            $mod.Target, $temporary, [IO.Compression.CompressionLevel]::Optimal, $false)
    }
    Move-Item -LiteralPath $temporary -Destination $output -Force
} finally {
    if (Test-Path -LiteralPath $temporary) { Remove-Item -LiteralPath $temporary -Force }
}
Write-Host "Packaged $output"
