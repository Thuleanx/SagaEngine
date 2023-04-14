#Sets minimum cmake version 
cmake_minimum_required(VERSION 3.14)

#Sets project name to CS1950UEngine
project(SagaEngine)

find_package(OpenGL REQUIRED)

#Tells compiler to use c++ 20
set(CMAKE_CXX_STANDARD 20)
#Makes c++ 20 required
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Setting path macros
set(GLFW_SOURCE_DIR "External/glfw-3.3.8")
set(GLEW_SOURCE_DIR "External/glew")
set(GLM_SOURCE_DIR "External/glm-master")
set(STB_SOURCE_DIR "External/stb")
set(IMGUI_SOURCE_DIR "External/imgui")
set(FREETYPE_SOURCE_DIR "External/freetype")
set(FMOD_DIR "C:/Program Files (x86)/FMOD SoundSystem/FMOD Studio API Windows")

#GLFW settings
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

#freetype settings
set(FT_DISABLE_ZLIB TRUE)
set(FT_DISABLE_BZIP2 TRUE)
set(FT_DISABLE_PNG TRUE)
set(FT_DISABLE_HARFBUZZ TRUE)
set(FT_DISABLE_BROTLI TRUE)

add_subdirectory(${GLFW_SOURCE_DIR})
add_subdirectory(${GLM_SOURCE_DIR})
add_subdirectory(${FREETYPE_SOURCE_DIR})

add_library(StaticGLEW STATIC ${GLEW_SOURCE_DIR}/src/glew.c)

# Add fmod
# add_library(fmod SHARED IMPORTED)
# set_property(TARGET fmod PROPERTY
#             IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/External/fmod.core/lib/x64/fmod.dll")
# set_property(TARGET fmod PROPERTY
#              IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/External/fmod/core/lib/x64/fmod_vc.lib")

# add_library(fmodstudio SHARED IMPORTED)
# set_property(TARGET fmodstudio PROPERTY
#              IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/External/fmod/studio/lib/x64/fmodstudio.dll")
# set_property(TARGET fmodstudio PROPERTY
#              IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/External/fmod/studio/lib/x64/fmodstudio_vc.lib")

include_directories(${GLFW_SOURCE_DIR}/include
                    ${GLFW_SOURCE_DIR}/deps
                    ${GLEW_SOURCE_DIR}/include
                    ${STB_SOURCE_DIR}
                    ${FREETYPE_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}
                    ${FMOD_DIR}/api/core/inc
                    ${FMOD_DIR}/api/studio/inc
                    )
# ImGui includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/External/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/External/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/External/stb
    )


link_directories(
	"${FMOD_SOURCE_DIR}/api/core/lib/x64"
	"${FMOD_STUDIO_SOURCE_DIR}/api/studio/lib/x64"
)

file(GLOB_RECURSE cppSources CONFIGURE_DEPENDS "Engine/*.cpp")
file(GLOB_RECURSE headerSources CONFIGURE_DEPENDS "Engine/*.h")
file(GLOB_RECURSE inlSources CONFIGURE_DEPENDS "Engine/*.inl")

file(GLOB_RECURSE appcppSources CONFIGURE_DEPENDS "Application/*.cpp")
file(GLOB_RECURSE appheaderSources CONFIGURE_DEPENDS "Application/*.h")
file(GLOB_RECURSE appinlSources CONFIGURE_DEPENDS "Application/*.inl")

add_executable(${PROJECT_NAME}
	${cppSources}
	${headerSources}
	${inlSources}
        ${appcppSources}
        ${appheaderSources}
        ${appinlSources}
    main.cpp

    Graphics/graphics.h
    Graphics/graphics.cpp
    Graphics/global.h
    Graphics/debug.h
    Graphics/camera.h
    Graphics/camera.cpp
    Graphics/shaderloader.h
    Graphics/material.h
    Graphics/material.cpp
    Graphics/shapedata.cpp
    Graphics/shapedata.h
    Graphics/shape.h
    Graphics/shape.cpp
    Graphics/modeltransform.h
    Graphics/modeltransform.cpp
    Graphics/light.h
    Graphics/light.cpp
    Graphics/font.h
    Graphics/font.cpp
    Graphics/textrenderer.h
    Graphics/textrenderer.cpp
    Graphics/GLWrappers/shader.h
    Graphics/GLWrappers/shader.cpp
    Graphics/GLWrappers/texture.h
    Graphics/GLWrappers/texture.cpp
    Graphics/GLWrappers/vbo.h
    Graphics/GLWrappers/vbo.cpp
    Graphics/GLWrappers/vao.h
    Graphics/GLWrappers/vao.cpp

    # IMGUI files
    External/imgui/imconfig.h
    External/imgui/imgui.cpp
    External/imgui/imgui.h
    External/imgui/imgui_demo.cpp
    External/imgui/imgui_draw.cpp
    External/imgui/imgui_internal.h
    External/imgui/imgui_tables.cpp
    External/imgui/imgui_widgets.cpp
    External/imgui/imstb_rectpack.h
    External/imgui/imstb_textedit.h
    External/imgui/imstb_truetype.h
    External/imgui/backends/imgui_impl_opengl3.cpp
    External/imgui/backends/imgui_impl_opengl3.h
    External/imgui/backends/imgui_impl_opengl3_loader.h
    External/imgui/backends/imgui_impl_glfw.cpp
    External/imgui/backends/imgui_impl_glfw.h

    External/stb/stb_image.h
)

target_link_libraries(${PROJECT_NAME} glfw StaticGLEW glm freetype ${OPENGL_LIBRARIES})
# target_link_libraries(${PROJECT_NAME} 
# 	${CMAKE_CURRENT_SOURCE_DIR}/External/fmod/core/lib/x86/libfmod.a 
# 	${CMAKE_CURRENT_SOURCE_DIR}/External/fmod/studio/lib/x86/libfmodstudio.a)
# target_link_libraries(${PROJECT_NAME} fmodstudio fmod)
target_link_libraries(${PROJECT_NAME} 
	${FMOD_DIR}/api/core/lib/x64/fmod_vc.lib
	${FMOD_DIR}/api/studio/lib/x64/fmodstudio_vc.lib)

if (WIN32)
  add_compile_definitions(GLEW_STATIC)
  target_link_libraries(${PROJECT_NAME}
      opengl32
      glu32
  )
endif()

if (UNIX AND NOT APPLE)
  target_link_libraries(${PROJECT_NAME}
      GL
  )
endif()
