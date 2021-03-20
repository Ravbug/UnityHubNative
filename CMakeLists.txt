cmake_minimum_required(VERSION 3.17)   

project("UnityHubNative")
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)

file(GLOB source "source/*.cpp" "source/*.hpp" "source/*.h")
if(APPLE)
    set(macfiles "source/wxmac.icns" CACHE INERNAL "")
elseif(WIN32)
	set(winfiles "windows.rc" CACHE INTERNAL "")
endif()
add_executable("${PROJECT_NAME}" WIN32 ${source} ${macfiles} ${winfiles})

#wxwidgets
set(wxBUILD_SHARED OFF CACHE INTERNAL "")
add_subdirectory("wxWidgets")

target_link_libraries("${PROJECT_NAME}"
    PUBLIC 
    wx::base
	wx::core
	wx::webview
)

# mac app
set_target_properties("${PROJECT_NAME}" PROPERTIES 
    MACOSX_BUNDLE ON
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_LIST_DIR}/source/Info.plist"
)
set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/source/wxmac.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
INSTALL(CODE 
		"include(BundleUtilities)
		fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/$<CONFIGURATION>/${APPNAME}.app\" \"\" \"\")
		" 
		COMPONENT Runtime
	)

# windows app
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DPI_AWARE "PerMonitor")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "${PROJECT_NAME}")
