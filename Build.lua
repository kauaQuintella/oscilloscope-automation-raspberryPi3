-- premake5.lua
workspace "OscilloscopeAutomation"
   configurations { "Debug", "Release", "Dist" }
   startproject "App"

filter "system:linux"
    toolset "gcc"

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Core"
	include "Core/Build-Core.lua"
group ""

include "App/Build-App.lua"