import os, sys
from pathlib import Path

Debug = True
Config = "Debug" if Debug else "Release"
MingRoot = Path(__file__).parent
MingCMakeLists = MingRoot / "CMakeLists.txt"

TopRoot = MingRoot.parent.parent.parent
BuildRoot = TopRoot / "build"


def make_project(
    name,
    save_path,
    srcs: list,
    includes: list = None,
    libs: list = None,
    defines: list = None,
    project="Ming",
):
    contents = []

    if srcs and len(srcs) > 0:
        srcs = " \n".join([f'"{x}"' for x in srcs])
        contents.append(f"add_executable({name} \n{srcs}\n)")

    if libs and len(libs) > 0:
        libs = " \n".join([f'"{x}"' for x in libs])
        contents.append(f"target_link_libraries({name} PRIVATE \n{libs}\n)")

    if includes and len(includes) > 0:
        includes = " \n".join([f'"{x}"' for x in includes])
        contents.append(f"target_include_directories({name} PRIVATE \n{includes}\n)")

    if defines and len(defines) > 0:
        defines = " \n".join([f"-D{x}" for x in defines])
        contents.append(f"target_compile_definitions({name} PRIVATE \n{defines}\n)")

    CMakeTemplate = [
        "cmake_minimum_required(VERSION 3.10)",
        f"project({project} CXX)",
        *contents,
    ]

    open(save_path, mode="w", encoding="utf-8").write("\n".join(CMakeTemplate))


def create_ming_project():
    Includes = r"""
DiligentCore/Common/interface
DiligentCore/Graphics/GraphicsAccessories/interface
DiligentCore/Graphics/GraphicsEngineD3D11/interface
DiligentCore/Graphics/GraphicsEngineD3D12/interface
DiligentCore/Graphics/GraphicsEngineD3DBase/interface
DiligentCore/Graphics/GraphicsEngineOpenGL/interface
DiligentCore/Graphics/GraphicsEngineVulkan/interface
DiligentCore/Graphics/GraphicsEngine/interface
DiligentCore/Graphics/GraphicsTools/interface
DiligentCore/Platforms/Basic/interface
DiligentCore/Platforms/Win32/interface
DiligentCore/Platforms/interface
DiligentCore/Primitives/interface
DiligentSamples/SampleBase/include
DiligentSamples/Tutorials/Tutorial01_HelloTriangle/src
DiligentTools/Imgui/interface
DiligentTools/NativeApp/include
DiligentTools/NativeApp/include/Win32
DiligentTools/TextureLoader/interface
DiligentTools/ThirdParty/imgui
"""

    Libs = r"""
DiligentCore/Common/Debug/Diligent-Common.lib
DiligentCore/Graphics/Archiver/Debug/Diligent-Archiver-static.lib
DiligentCore/Graphics/GraphicsAccessories/Debug/Diligent-GraphicsAccessories.lib
DiligentCore/Graphics/GraphicsEngineD3D11/Debug/Diligent-GraphicsEngineD3D11-static.lib
DiligentCore/Graphics/GraphicsEngineD3D11/Debug/GraphicsEngineD3D11_64d.lib
DiligentCore/Graphics/GraphicsEngineD3D12/Debug/Diligent-GraphicsEngineD3D12-static.lib
DiligentCore/Graphics/GraphicsEngineD3D12/Debug/GraphicsEngineD3D12_64d.lib
DiligentCore/Graphics/GraphicsEngineD3DBase/Debug/Diligent-GraphicsEngineD3DBase.lib
DiligentCore/Graphics/GraphicsEngineNextGenBase/Debug/Diligent-GraphicsEngineNextGenBase.lib
DiligentCore/Graphics/GraphicsEngineOpenGL/Debug/Diligent-GraphicsEngineOpenGL-static.lib
DiligentCore/Graphics/GraphicsEngineOpenGL/Debug/GraphicsEngineOpenGL_64d.lib
DiligentCore/Graphics/GraphicsEngineVulkan/Debug/Diligent-GraphicsEngineVk-static.lib
DiligentCore/Graphics/GraphicsEngineVulkan/Debug/GraphicsEngineVk_64d.lib
DiligentCore/Graphics/GraphicsEngine/Debug/Diligent-GraphicsEngine.lib
DiligentCore/Graphics/GraphicsTools/Debug/Diligent-GraphicsTools.lib
DiligentCore/Graphics/HLSL2GLSLConverterLib/Debug/Diligent-HLSL2GLSLConverterLib.lib
DiligentCore/Graphics/ShaderTools/Debug/Diligent-ShaderTools.lib
DiligentCore/Platforms/Basic/Debug/Diligent-BasicPlatform.lib
DiligentCore/Platforms/Win32/Debug/Diligent-Win32Platform.lib
DiligentCore/Primitives/Debug/Diligent-Primitives.lib
DiligentCore/ThirdParty/SPIRV-Cross/Debug/spirv-cross-cored.lib
DiligentCore/ThirdParty/SPIRV-Cross/Debug/spirv-cross-glsld.lib
DiligentCore/ThirdParty/SPIRV-Tools/source/Debug/SPIRV-Tools.lib
DiligentCore/ThirdParty/SPIRV-Tools/source/opt/Debug/SPIRV-Tools-opt.lib
DiligentCore/ThirdParty/glew/Debug/glew-static.lib
DiligentCore/ThirdParty/glslang/SPIRV/Debug/SPIRVd.lib
DiligentCore/ThirdParty/glslang/glslang/Debug/glslangd.lib
DiligentCore/ThirdParty/volk/Debug/volk.lib
DiligentCore/ThirdParty/xxHash/build/cmake/Debug/xxhash.lib
DiligentTools/Imgui/Debug/Diligent-Imgui.lib
DiligentTools/NativeApp/Debug/Diligent-NativeAppBase.lib
DiligentTools/TextureLoader/Debug/Diligent-TextureLoader.lib
DiligentTools/ThirdParty/Debug/ZLib.lib
DiligentTools/ThirdParty/libjpeg-9e/Debug/LibJpeg.lib
DiligentTools/ThirdParty/libpng/Debug/libpng16_staticd.lib
DiligentTools/ThirdParty/libtiff/Debug/LibTiff.lib
DiligentSamples/SampleBase/Debug/Diligent-SampleBase.lib
"""

    LibSystem = [
        "Shlwapi.lib",
        "d3d11.lib",
        "d3dcompiler.lib",
        "dxgi.lib",
        "opengl32.lib",
    ]

    Defines = """
D3D11_SUPPORTED=1
D3D12_SUPPORTED=1
DEBUG
DILIGENT_DEBUG
DILIGENT_DEVELOPMENT
DILIGENT_RENDER_STATE_CACHE_SUPPORTED=1
ENGINE_DLL=1
GLES_SUPPORTED=0
GL_SUPPORTED=1
METAL_SUPPORTED=0
PLATFORM_WIN32=1
VULKAN_SUPPORTED=1
WEBGPU_SUPPORTED=0
"""

    Includes = [
        Path(f"{TopRoot}/{x.strip()}").absolute().as_posix()
        for x in Includes.split("\n")
        if x.strip() != ""
    ]

    Libs = [
        str(Path(f"{BuildRoot}/{x.strip()}").absolute().as_posix()).replace(
            "/Debug", f"/{Config}"
        )
        for x in Libs.split("\n")
        if x.strip() != ""
    ]

    Defines = [x.strip() for x in Defines.split("\n") if x.strip() != ""]

    make_project(
        name="M_01_Triangle",
        srcs=["M_01_Triangle.cpp"],
        save_path=MingCMakeLists,
        includes=Includes,
        libs=Libs + LibSystem,
        defines=Defines,
    )
