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

# Find wxWidgets - works with both vcpkg CONFIG mode and system FindwxWidgets module
# Try CONFIG mode first (vcpkg provides this)
find_package(wxWidgets CONFIG QUIET COMPONENTS core base stc)

if(NOT wxWidgets_FOUND)
    # Fallback to module mode for system-installed wxWidgets (common on Linux/macOS)
    message(STATUS "wxWidgets not found via CONFIG mode, trying MODULE mode...")
    find_package(wxWidgets COMPONENTS core base stc)

    if(wxWidgets_FOUND)
        include(${wxWidgets_USE_FILE})
    else()
        message(FATAL_ERROR
            "wxWidgets not found!\n"
            "Please install wxWidgets using one of these methods:\n"
            "  - Windows (vcpkg): Set VCPKG_ROOT environment variable and run 'vcpkg install wxwidgets:x64-windows'\n"
            "  - Linux: Install via package manager (e.g., 'sudo apt install libwxgtk3.0-gtk3-dev')\n"
            "  - macOS: Install via Homebrew ('brew install wxwidgets')\n"
            "Or ensure CMAKE_TOOLCHAIN_FILE points to vcpkg.cmake if using vcpkg."
        )
    endif()
endif()

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

# Link wxWidgets (different syntax depending on how it was found)
if(TARGET wx::core)
    # vcpkg / CONFIG mode
    target_link_libraries(AstraIRC PRIVATE
        wx::core
        wx::base
        wx::stc
    )
else()
    # Module mode (system wxWidgets) - wxWidgets_LIBRARIES includes all components
    target_link_libraries(AstraIRC PRIVATE ${wxWidgets_LIBRARIES})
endif()

# Platform-specific libraries
if(WIN32)
    target_link_libraries(AstraIRC PRIVATE ws2_32)
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
