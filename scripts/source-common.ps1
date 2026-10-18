param(
    [Parameter(Mandatory)][ValidatePattern('^[a-z0-9]+(-[a-z0-9]+)*$')][string]$Id,
    [Parameter(Mandatory)][string]$Name,
    [Parameter(Mandatory)][ValidateSet('mod', 'nvse-plugin', 'fomod')][string]$Kind,
    [string]$Project = '', [string]$Dll = '', [string]$TestProperty = ''
)

# Environment variables survive running the selector with either . or &.
$env:MOD_WORKSPACE = Split-Path -Parent $PSScriptRoot
$env:MOD_ID = $Id
$env:MOD_NAME = $Name
$env:MOD_KIND = $Kind
$env:MOD_PROJECT = $Project
$env:MOD_DLL = $Dll
$env:MOD_TEST_PROPERTY = $TestProperty
Write-Host "Selected $Name ($Id)"
