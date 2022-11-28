cmake_minimum_required(VERSION 3.16)   

project("UnityHubNative")
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>)

option(BUILD_SHARED_LIBS OFF "Build static")

if(MSVC)
  add_compile_options(/MP)
endif()

set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE INTERNAL "")
# linux detection
if(UNIX AND NOT APPLE)
	set(LINUX TRUE CACHE INTERNAL "")
	add_definitions(-Wno-write-strings)
endif()
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/wxWidgets/build/cmake/modules" CACHE INTERNAL "")
message("module path = ${CMAKE_MODULE_PATH}")

file(GLOB source "source/*.cpp" "source/*.hpp" "source/*.h")
if (APPLE)
	set(OBJCPP "source/AppleUtilities.h" "source/AppleUtilities.mm")
endif()
add_executable("${PROJECT_NAME}" WIN32 ${source} "source/wxmac.icns" "source/windows.rc" ${OBJCPP})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

#wxwidgets
set(wxBUILD_SHARED OFF CACHE INTERNAL "")
set(wxUSE_STL ON CACHE INTERNAL "")
set(wxUSE_REGEX OFF CACHE INTERNAL "")
set(wxUSE_LIBJPEG OFF CACHE INTERNAL "")
set(wxUSE_LIBTIFF OFF CACHE INTERNAL "")
set(wxUSE_ZLIB builtin CACHE INTERNAL "")
add_subdirectory("wxWidgets" EXCLUDE_FROM_ALL)

# libcurl
if (MSVC)
    # use Windows native SSL instead of OpenSSL or MbedTLS
    set(CURL_USE_SCHANNEL ON CACHE INTERNAL "")
else()
    # for curl SSL
    set(ENABLE_TESTING OFF CACHE INTERNAL "")
    set(ENABLE_PROGRAMS OFF CACHE INTERNAL "")
    set(CURL_USE_MBEDTLS ON CACHE INTERNAL "")
    add_subdirectory(mbedtls EXCLUDE_FROM_ALL)
    set(MBEDTLS_LIBRARIES "mbedtls")   
endif()
set(BUILD_TESTING OFF)
set(USE_OPENSSL OFF)
set(CURL_USE_LIBSSH2 OFF)
set(CURL_DISABLE_FTP ON)      # disable unused features & protocols
set(CURL_DISABLE_FILE ON)
set(CURL_DISABLE_TELNET ON)
set(CURL_DISABLE_LDAP ON)
set(CURL_DISABLE_DICT ON)
set(CURL_DISABLE_TFTP ON)
set(CURL_DISABLE_GOPHER ON)
set(CURL_DISABLE_IMAP ON)
set(CURL_DISABLE_SMB ON)
set(CURL_DISABLE_SMTP ON)
set(CURL_DISABLE_RTSP ON)
set(CURL_DISABLE_MQTT ON)
set(CURL_DISABLE_POP3 ON)
add_subdirectory(curl EXCLUDE_FROM_ALL)

add_subdirectory("fmt" EXCLUDE_FROM_ALL)

target_link_libraries("${PROJECT_NAME}"
    PUBLIC 
	wx::core
	CURL::libcurl
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
	"include(${CMAKE_CURRENT_SOURCE_DIR}/appimage.cmake)
	make_appimage(
		EXE \"${CMAKE_INSTALL_PREFIX}/$<CONFIGURATION>/${PROJECT_NAME}\"
		NAME \"${PROJECT_NAME}\"
		ICON \"${CMAKE_CURRENT_SOURCE_DIR}/source/wxlin.xpm\"
		DIR_ICON \"${CMAKE_CURRENT_SOURCE_DIR}/source/wxlin.xpm\"
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
 if(LINUX)
	 add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} ${CMAKE_BUILD_TYPE}/${PROJECT_NAME})
 elseif(APPLE)
	 add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} ${CMAKE_BUILD_TYPE}/${PROJECT_NAME}.app/Contents/MacOS/${PROJECT_NAME})
 endif()
endif ()

macro(enable_unity targets)
	foreach(target ${targets})
		set_target_properties("${target}" PROPERTIES UNITY_BUILD ON)
	endforeach()
endmacro()

set(all_unity "wxcore;wxbase;fmt;wxpng;")
if(MSVC)
#enable_unity("${all_unity}")
endif()
