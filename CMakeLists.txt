cmake_minimum_required(VERSION 3.16)   

project("UnityHubNative")
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)

if(MSVC)
  add_definitions(/MP)
  add_definitions(/Qpar)
endif()

set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE INTERNAL "")
# linux detection
if(UNIX AND NOT APPLE)
	set(LINUX TRUE CACHE INTERNAL "")
endif()

file(GLOB source "source/*.cpp" "source/*.hpp" "source/*.h")
add_executable("${PROJECT_NAME}" WIN32 ${source} "source/wxmac.icns" "source/windows.rc")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

#wxwidgets
set(wxBUILD_SHARED OFF CACHE INTERNAL "")
add_subdirectory("wxWidgets" EXCLUDE_FROM_ALL)

set(CPR_BUILD_TESTS OFF CACHE INTERNAL "")
set(CPR_GENERATE_COVERAGE OFF CACHE INTERNAL "")
add_subdirectory("cpr" EXCLUDE_FROM_ALL)

add_subdirectory("fmt" EXCLUDE_FROM_ALL)

target_link_libraries("${PROJECT_NAME}"
    PUBLIC 
    wx::base
	wx::core
	wx::webview
	cpr
	fmt
)
target_include_directories(${PROJECT_NAME} PUBLIC "fmt/include")

# mac app
set_target_properties("${PROJECT_NAME}" PROPERTIES 
    MACOSX_BUNDLE ON
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_LIST_DIR}/source/Info.plist"
	XCODE_GENERATE_SCHEME ON
)
set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/source/wxmac.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
if(APPLE)
	INSTALL(CODE 
		"include(BundleUtilities)
		fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/$<CONFIGURATION>/${PROJECT_NAME}.app\" \"\" \"\")
		" 
		COMPONENT Runtime
	)
elseif(LINUX)
INSTALL(CODE 
	"include(../../appimage.cmake)
	make_appimage(
		EXE \"${CMAKE_INSTALL_PREFIX}/$<CONFIGURATION>/${PROJECT_NAME}\"
		NAME \"${PROJECT_NAME}\"
		ICON \"wxlin\"
		DIR_ICON \"wxlin.xpm\"
		OUTPUT_NAME \"${CMAKE_INSTALL_PREFIX}/$<CONFIGURATION>/${PROJECT_NAME}.AppImage\"
		ASSETS \"\"
	)
	" 
	COMPONENT Runtime
	)
endif()
# windows app
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DPI_AWARE "PerMonitor")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "${PROJECT_NAME}")

 # Strip binary for release builds
 if (CMAKE_BUILD_TYPE STREQUAL Release)
 if(LINUX OR MSVC)
	 add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} ${CMAKE_BUILD_TYPE}/${PROJECT_NAME})
 elseif(APPLE)
	 add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} ${CMAKE_BUILD_TYPE}/${PROJECT_NAME}.app/Contents/MacOS/${PROJECT_NAME})
 endif()
endif ()
