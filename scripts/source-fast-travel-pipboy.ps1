# Select this package for the shared build and package scripts.
. (Join-Path $PSScriptRoot 'source-common.ps1') -Id 'fast-travel-pipboy' -Name 'Keep Pip-Boy Open During Fast Travel' -Kind 'nvse-plugin' -Project 'nvse_plugin_keep_pipboy_open_during_fast_travel.vcxproj' -Dll 'keep_pipboy_open_during_fast_travel.dll' -TestProperty 'RunTests'
