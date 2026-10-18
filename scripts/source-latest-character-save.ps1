# Select this package for the shared build and package scripts.
. (Join-Path $PSScriptRoot 'source-common.ps1') -Id 'latest-character-save' -Name 'Only Load Latest Save Per Character' -Kind 'nvse-plugin' -Project 'nvse_plugin_only_load_latest_save_per_character.vcxproj' -Dll 'only_load_latest_save_per_character.dll' -TestProperty 'GuardTests'
