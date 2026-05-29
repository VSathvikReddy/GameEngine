# ==============================================================================
# 1. Project Setup
# ==============================================================================
cmake_minimum_required(VERSION 3.15)
project(GameEngine VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

# ==============================================================================
# 2. Fully Automated Dependency Fetching
# ==============================================================================
include(FetchContent)

# --- GLFW (Windowing) ---
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4 
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

# --- GLM (Math Library) ---
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        0.9.9.8
)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)

# --- STB (Image Loading) ---
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
)

# --- GLEW (OpenGL Extensions) ---
# We download the official ZIP release instead of Git so we get the pre-generated glew.c
FetchContent_Declare(
    glew
    URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip
)

# Tell CMake to download everything right now
FetchContent_MakeAvailable(glfw glm stb glew)

# --- OpenGL ---
find_package(OpenGL REQUIRED)

# ==============================================================================
# 3. Source Files and Executable
# ==============================================================================

# Find your engine's C++ files
file(GLOB_RECURSE ENGINE_SRCS CONFIGURE_DEPENDS "src/*.cpp")

# Grab the raw glew.c file directly from the downloaded GLEW folder
set(GLEW_SRC "${glew_SOURCE_DIR}/src/glew.c")

# Create the executable
add_executable(game ${ENGINE_SRCS} ${GLEW_SRC})

# ==============================================================================
# 4. Includes and Linking
# ==============================================================================

target_include_directories(game PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include # Your engine headers
    ${stb_SOURCE_DIR}                   # Downloaded STB headers
    ${glew_SOURCE_DIR}/include          # Downloaded GLEW headers
)

target_compile_options(game PRIVATE 
    -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor
)

# Note: GLM creates a CMake target called 'glm::glm', so we link it here 
# instead of adding it to target_include_directories.
target_link_libraries(game PRIVATE
    OpenGL::GL 
    glfw        
    glm::glm    
)

# ==============================================================================
# 5. Asset Management
# ==============================================================================
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})