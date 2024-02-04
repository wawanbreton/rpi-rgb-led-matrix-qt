cmake_minimum_required(VERSION 3.22)
project(rpi-rgb-led-matrix)

# Declare and check variables for linking to rpi-rgb-led-matrix library
set(LIBRGBMATRIX_INCLUDE_DIR "" CACHE PATH "Path to the folder containing the rpi-rgb-led-matrix headers")
set(LIBRGBMATRIX_LIBRARY_DIR "" CACHE PATH "Path to the folder containing the rpi-rgb-led-matrix library")

if("${LIBRGBMATRIX_INCLUDE_DIR}" STREQUAL "")
    message(FATAL_ERROR "Please define LIBRGBMATRIX_INCLUDE_DIR")
endif()

if("${LIBRGBMATRIX_LIBRARY_DIR}" STREQUAL "")
    message(FATAL_ERROR "Please define LIBRGBMATRIX_LIBRARY_DIR")
endif()

# Include freetype for text rendering
find_package(Freetype REQUIRED)

# Include required Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

# Declare plugin generation
qt_add_plugin(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME}
    PRIVATE
    rpiggbledmatrixqtplugin.cpp rpiggbledmatrixqtplugin.h
    ledmatrixbackingstore.cpp ledmatrixbackingstore.h
    ledmatrixintegration.cpp ledmatrixintegration.h
    ledmatrixscreen.cpp ledmatrixscreen.h
    driveroptions.h
)

# Include headers for rpi-rgb-led-matrix
include_directories(${LIBRGBMATRIX_INCLUDE_DIR})

# Include headers for freetype
include_directories(${FREETYPE_INCLUDE_DIRS})

# Include headers for Qt. This requires a bit of tricking because QPlatformIntegrationPlugin
# generation requires private Qt headers.
list(GET Qt6Core_INCLUDE_DIRS 0 QtCore_INCLUDE_DIR)
include_directories(${PROJECT_NAME} ${QtCore_INCLUDE_DIR}/${Qt6Core_VERSION})
include_directories(${PROJECT_NAME} ${QtCore_INCLUDE_DIR}/${Qt6Core_VERSION}/QtCore)

list(GET Qt6Gui_INCLUDE_DIRS 0 QtGui_INCLUDE_DIR)
include_directories(${PROJECT_NAME} ${QtGui_INCLUDE_DIR}/${Qt6Core_VERSION})
include_directories(${PROJECT_NAME} ${QtGui_INCLUDE_DIR}/${Qt6Core_VERSION}/QtGui)

# Link instructions
target_link_directories(${PROJECT_NAME} PRIVATE ${LIBRGBMATRIX_LIBRARY_DIR})
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    ${FREETYPE_LIBRARIES}
    rgbmatrix
)
