# SoulEngine OpenGL dependency setup
# This module normalizes OpenGL-related dependencies into an abstract target
#   SoulEngine::OpenGLDeps

if (NOT SOULENGINE_WITH_OPENGL)
  return()
endif()
message("Including OpenGL dependencies")
option(SOULENGINE_FETCH_THIRDPARTY "Allow fetching third-party deps" ON)

# Keep the VS solution tidy: place everything created here under thirdlib/OpenGL
set(_SE_PREV_CMAKE_FOLDER "${CMAKE_FOLDER}")
set(CMAKE_FOLDER "thirdlib/OpenGL")

# Prefer an existing glfw package first
find_package(glfw3 CONFIG QUIET)

if (NOT TARGET glfw)
  if (SOULENGINE_FETCH_THIRDPARTY)
    include(FetchContent)
    message(STATUS "SoulEngine: fetching glfw via FetchContent")
    FetchContent_Declare(
      glfw
      GIT_REPOSITORY https://github.com/glfw/glfw.git
      GIT_TAG 3.4
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS   OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS    OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL       OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)
  else()
    message(FATAL_ERROR "glfw3 not found and SOULENGINE_FETCH_THIRDPARTY=OFF")
  endif()
endif()

# Add GLAD from ThirdParty if present
set(_SE_GLAD_ROOT "${CMAKE_SOURCE_DIR}/ThirdParty/glad")
if (EXISTS "${_SE_GLAD_ROOT}/src/gl.c" AND NOT TARGET glad)
  # Enable C language support
  enable_language(C)
  add_library(glad STATIC "${_SE_GLAD_ROOT}/src/gl.c")
  target_include_directories(glad PUBLIC "${_SE_GLAD_ROOT}/include")
  set_target_properties(glad PROPERTIES 
    FOLDER "thirdlib/OpenGL"
    LINKER_LANGUAGE C
  )
endif()

# Find system OpenGL library
find_package(OpenGL QUIET)
if (NOT OpenGL_FOUND AND WIN32)
  # On Windows, OpenGL32 is the usual lib
  set(OpenGL_GL_PREFERENCE GLVND)
  set(OPENGL_gl_LIBRARY opengl32)
endif()

# Expose a stable interface target for the engine to link against
if (NOT TARGET SoulEngine_OpenGLDeps)
  add_library(SoulEngine_OpenGLDeps INTERFACE)
  set_target_properties(SoulEngine_OpenGLDeps PROPERTIES FOLDER "thirdlib/OpenGL")
  # Link glfw + glad + OpenGL
  target_link_libraries(SoulEngine_OpenGLDeps INTERFACE
    glfw
    $<IF:$<TARGET_EXISTS:glad>,glad,>
    $<IF:$<TARGET_EXISTS:OpenGL::GL>,OpenGL::GL,${OPENGL_gl_LIBRARY}>
  )
  add_library(SoulEngine::OpenGLDeps ALIAS SoulEngine_OpenGLDeps)
endif()

# Override GLFW's own folder assignment so it appears under thirdlib/OpenGL
if (TARGET glfw)
  set_target_properties(glfw PROPERTIES FOLDER "thirdlib/OpenGL")
endif()
if (TARGET glfw3)
  set_target_properties(glfw3 PROPERTIES FOLDER "thirdlib/OpenGL")
endif()
if (TARGET glfw3::glfw)
  get_target_property(_se_real glfw3::glfw ALIASED_TARGET)
  if(_se_real)
    set_target_properties(${_se_real} PROPERTIES FOLDER "thirdlib/OpenGL")
  endif()
endif()
if (TARGET GLFW::GLFW)
  get_target_property(_se_real GLFW::GLFW ALIASED_TARGET)
  if(_se_real)
    set_target_properties(${_se_real} PROPERTIES FOLDER "thirdlib/OpenGL")
  endif()
endif()

# Restore previous folder setting for subsequent targets
set(CMAKE_FOLDER "${_SE_PREV_CMAKE_FOLDER}")
if (TARGET update_mappings)
  set_target_properties(update_mappings PROPERTIES FOLDER "thirdlib/OpenGL")
endif()
