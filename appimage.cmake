function(make_appimage)
	set(optional)
	set(args EXE NAME ICON OUTPUT_NAME)
	set(list_args ASSETS)
	cmake_parse_arguments(
		PARSE_ARGV 0
		ARGS
		"${optional}"
		"${args}"
		"${list_args}"
	)

	if(${ARGS_UNPARSED_ARGUMENTS})
		message(WARNING "Unparsed arguments: ${ARGS_UNPARSED_ARGUMENTS}")
	endif()


    # download AppImageTool if needed (TODO: non-x86 build machine?)
    SET(AIT_PATH "${CMAKE_BINARY_DIR}/AppImageTool.AppImage" CACHE INTERNAL "")
    if (NOT EXISTS "${AIT_PATH}")
        file(DOWNLOAD https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage "${AIT_PATH}")
        execute_process(COMMAND chmod +x ${AIT_PATH})
    endif()

    # make the AppDir and icons directory
    set(APPDIR "${CMAKE_BINARY_DIR}/AppDir")
    set(RICONDIR "usr/share/icons/hicolor/512x512/apps") # relative icon dir
    set(ICONDIR "${APPDIR}/${RICONDIR}")
    file(REMOVE_RECURSE "${APPDIR}")       # remove if leftover
    file(MAKE_DIRECTORY "${APPDIR}")
    file(MAKE_DIRECTORY "${ICONDIR}")

    # copy executable to appdir
    file(COPY "${ARGS_EXE}" DESTINATION "${APPDIR}" FOLLOW_SYMLINK_CHAIN)
    get_filename_component(EXE_NAME "${ARGS_EXE}" NAME)

    # create the script that will launch the AppImage
file(WRITE "${APPDIR}/AppRun" 
"#!/bin/sh
cd \"$(dirname \"$0\")\";
./${EXE_NAME} $@"
    )
    execute_process(COMMAND chmod +x "${APPDIR}/AppRun")
    
    # copy assets to appdir
    file(COPY ${ARGS_ASSETS} DESTINATION "${APPDIR}")

    # copy icon
    file(COPY ${ARGS_ICON} DESTINATION "${ICONDIR}")
    get_filename_component(ICON_NAME "${ARGS_ICON}" NAME)
    get_filename_component(ICON_EXT "${ARGS_ICON}" EXT)
    file(RENAME "${ICONDIR}/${ICON_NAME}" "${ICONDIR}/${ARGS_NAME}${ICON_EXT}")

    # create icon symlinks
    file(CREATE_LINK "${RICONDIR}/${ARGS_NAME}${ICON_EXT}" "${APPDIR}/.DirIcon" SYMBOLIC)
    file(CREATE_LINK "${RICONDIR}/${ARGS_NAME}${ICON_EXT}" "${APPDIR}/${ARGS_NAME}${ICON_EXT}" SYMBOLIC)

    # Create the .desktop file
    file(WRITE "${APPDIR}/${ARGS_NAME}.desktop" 
    "[Desktop Entry]
Type=Application
Name=${ARGS_NAME}
Icon=${ARGS_NAME}
Exec=AppRun
Categories=X-None;"    
    )

    # Invoke AppImageTool
    execute_process(COMMAND ${AIT_PATH} ${APPDIR} ${ARGS_OUTPUT_NAME})
    file(REMOVE_RECURSE "${APPDIR}")

endfunction()

