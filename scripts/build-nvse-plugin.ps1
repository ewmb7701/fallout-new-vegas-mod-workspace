[CmdletBinding()]
param(
    [ValidateSet('Debug', 'Release')][string]$Configuration = 'Release',
    [switch]$Test
)
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'common.ps1')
$mod = Get-ModContext
if ($mod.Kind -ne 'nvse-plugin') { throw 'Use build-mod.ps1 for data mods and FOMODs.' }
$project = Join-Path $mod.Source $mod.Project
if (!(Test-Path -LiteralPath $project)) { throw "Missing project: $project" }
if ($Test) {
    if (!$mod.TestProperty) { throw 'This plugin does not define a native test target.' }
    $testSource = if ($mod.TestProperty -eq 'RunTests') { 'tests.cpp' } else { 'tests\integration.cpp' }
    if (!(Test-Path -LiteralPath (Join-Path $mod.Source $testSource))) {
        throw "The imported project does not contain $testSource; native tests are unavailable."
    }
}
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$installation = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (!$installation) { throw 'Install Visual Studio with Desktop development with C++.' }
$msbuild = Join-Path $installation 'MSBuild\Current\Bin\MSBuild.exe'
$arguments = @($project, '/m', '/t:Build', "/p:Configuration=$Configuration", '/p:Platform=Win32', '/p:PostBuildEventUseInBuild=false', '/v:minimal')
Reset-ModTarget $mod
& $msbuild @arguments
if ($LASTEXITCODE -ne 0) { throw 'Plugin build failed.' }
$binaryDir = $mod.Target
$dll = Join-Path $binaryDir $mod.Dll
if (!(Test-Path -LiteralPath $dll)) { throw "Missing build output: $dll" }
if ($Test) {
    & $msbuild @arguments "/p:$($mod.TestProperty)=true"
    if ($LASTEXITCODE -ne 0) { throw 'Native test build failed.' }
    $testExe = Join-Path $binaryDir ('tests\' + [IO.Path]::GetFileNameWithoutExtension($mod.Dll) + '_tests.exe')
    if ($mod.TestProperty -eq 'GuardTests') {
        Push-Location (Split-Path -Parent $testExe)
        try {
            foreach ($mode in @('vanilla', 'stewie', 'conflict', 'sound-conflict', 'appearance-conflict', 'refresh-conflict')) {
                for ($attempt = 0; $attempt -lt 32; $attempt++) {
                    & $testExe $dll $mode
                    if ($LASTEXITCODE -ne 77) { break }
                }
                if ($LASTEXITCODE -ne 0) { throw "Native tests failed: $mode" }
            }
        } finally { Pop-Location }
    } else {
        & $testExe
        if ($LASTEXITCODE -ne 0) { throw 'Native tests failed.' }
    }
}
Write-Host "Built $($mod.Name): $($mod.Target)"
