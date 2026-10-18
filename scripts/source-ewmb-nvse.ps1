# Select this package for the shared build and package scripts.
. (Join-Path $PSScriptRoot 'source-common.ps1') -Id 'ewmb-nvse' -Name 'ewmb NVSE' -Kind 'nvse-plugin' -Project 'ewmb_nvse.vcxproj' -Dll 'ewmb_nvse.dll'
