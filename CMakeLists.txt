cmake_minimum_required(VERSION 3.22)
project(rpi-rgb-led-matrix)

include_directories(/usr/include/freetype2)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

#qt_find_package(WrapFreetype PROVIDED_TARGETS WrapFreetype::WrapFreetype)

qt_add_plugin(${PROJECT_NAME}
    SHARED
    #CLASS_NAME RpiRgbLedMatrixQtPlugin
    #PLUGIN_TYPE platforms
    rpiggbledmatrixqtplugin.cpp rpiggbledmatrixqtplugin.h
    ledmatrixbackingstore.cpp ledmatrixbackingstore.h
    ledmatrixintegration.cpp ledmatrixintegration.h
    ledmatrixscreen.cpp ledmatrixscreen.h
)

list(GET Qt6Core_INCLUDE_DIRS 0 QtCore_INCLUDE_DIR)
include_directories(${PROJECT_NAME} ${QtCore_INCLUDE_DIR}/${Qt6Core_VERSION})
include_directories(${PROJECT_NAME} ${QtCore_INCLUDE_DIR}/${Qt6Core_VERSION}/QtCore)

list(GET Qt6Gui_INCLUDE_DIRS 0 QtGui_INCLUDE_DIR)
include_directories(${PROJECT_NAME} ${QtGui_INCLUDE_DIR}/${Qt6Core_VERSION})
include_directories(${PROJECT_NAME} ${QtGui_INCLUDE_DIR}/${Qt6Core_VERSION}/QtGui)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets freetype)

# qt_internal_extend_target(QMinimalIntegrationPlugin CONDITION QT_FEATURE_freetype
#     LIBRARIES
#         WrapFreetype::WrapFreetype
# )
