workspace "Copper"
    architecture "ARM64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

IncludeDir = {}
IncludeDir["GLFW"] = "Copper/vendor/GLFW/include"
IncludeDir["Glad"] = "Copper/vendor/glad/include"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Copper/vendor/GLFW"
include "Copper/vendor/glad"

project "Copper"
    location "Copper"
    language "C++"
    cppdialect "C++17"
    kind "SharedLib"
    staticruntime "Off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "src/cprpch.h"
    pchsource "Copper/src/cprpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }
    
    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}"
    }

    links
    {
        "GLFW",
        "Glad"
    }

    defines
    {
        -- make it possible to use glad with GLFW
        "GLFW_INCLUDE_NONE"
    }

    filter "system:macosx"
        systemversion "latest"

        xcodebuildsettings
        {
            ["ALWAYS_SEARCH_USER_PATHS"] = "YES",
            -- the path where project it links to will search dynamic lib
            ["DYLIB_INSTALL_NAME_BASE"] = "../Copper"
        }

        links
        {
            "CoreFoundation.framework",
            "IOKit.framework",
            "Cocoa.framework",
            "OpenGL.framework"
        }

        defines
        {
            "CPR_PLATFORM_MACOS"
        }

    filter "configurations:Debug"
        defines
        {
            "CPR_DEBUG",
            "CPR_ENABLE_ASSERTS"
        }
        symbols "On"

    filter "configurations:Release"
        defines "CPR_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "CPR_DIST"
        optimize "On"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }
    
    includedirs
    {
        "Copper/src",
        "Copper/vendor/spdlog/include"
    }

    links
    {
        "Copper"
    }

    filter "system:macosx"
        systemversion "latest"

        xcodebuildsettings
        {
            ["ALWAYS_SEARCH_USER_PATHS"] = "YES"
        }

    filter "configurations:Debug"
        defines
        {
            "CPR_DEBUG",
            "CPR_ENABLE_ASSERTS"
        }
        symbols "On"

    filter "configurations:Release"
        defines "CPR_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "CPR_DIST"
        optimize "On"
        
