cmake_minimum_required(VERSION 3.20)
project(AstraIRC VERSION 1.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific executable settings
if(WIN32)
    # Creates a Windows GUI application (no console window)
    set(CMAKE_WIN32_EXECUTABLE ON)
elseif(APPLE)
    # Creates a macOS application bundle
    set(CMAKE_MACOSX_BUNDLE ON)
    set(MACOSX_BUNDLE_BUNDLE_NAME "AstraIRC")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.astrairc.app")
endif()

# Find wxWidgets using MODULE mode (works with both vcpkg and system installations)
# vcpkg's wxWidgets port uses FindwxWidgets.cmake (MODULE mode), not CONFIG mode
# Request the components we need: core (GUI), base (non-GUI), adv (advanced), aui (advanced UI), richtext (rich text control)
find_package(wxWidgets REQUIRED COMPONENTS core base adv aui richtext)

# Include wxWidgets configuration
include(${wxWidgets_USE_FILE})

# Source files
set(SOURCES
    src/main.cpp
    src/MainFrame.cpp
    src/MainFrame.h
    src/ServerConnectionPanel.cpp
    src/ServerConnectionPanel.h
    src/ChannelPage.cpp
    src/ChannelPage.h
    src/irc_core.cpp
    src/irc_core.h
    src/UserInfo.h
    src/UserProfileDialog.cpp
    src/UserProfileDialog.h
)

add_executable(AstraIRC ${SOURCES})

# Include directories
target_include_directories(AstraIRC PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Link wxWidgets libraries (MODULE mode provides wxWidgets_LIBRARIES variable)
target_link_libraries(AstraIRC PRIVATE ${wxWidgets_LIBRARIES})

# Platform-specific libraries and linking
if(WIN32)
    target_link_libraries(AstraIRC PRIVATE ws2_32)
elseif(UNIX AND NOT APPLE)
    # Static link C++ standard library for better Linux compatibility
    # This avoids GLIBCXX version conflicts on older distributions
    target_link_options(AstraIRC PRIVATE -static-libgcc -static-libstdc++)
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(AstraIRC PRIVATE /W4)
else()
    target_compile_options(AstraIRC PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install rules
install(TARGETS AstraIRC
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
