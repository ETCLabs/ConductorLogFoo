cmake_minimum_required(VERSION 3.15.3)
project(ConductorLogFoo VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ui/)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Sql Core )

add_executable(ConductorLogFoo [WIN32]
    src/main.cpp
    src/mainwindow.cpp
    src/queryrunner.cpp
    src/calendarrangewidget.cpp
    ConductorLogFoo.rc
)

target_link_libraries(ConductorLogFoo Qt5::Widgets Qt5::Core Qt5::Sql)

# WinDeployQt, see https://stackoverflow.com/questions/41193584/deploy-all-qt-dependencies-when-building
if(Qt5_FOUND AND WIN32 AND TARGET Qt5::qmake AND NOT TARGET Qt5::windeployqt)
    get_target_property(_qt5_qmake_location Qt5::qmake IMPORTED_LOCATION)

    execute_process(
        COMMAND "${_qt5_qmake_location}" -query QT_INSTALL_PREFIX
        RESULT_VARIABLE return_code
        OUTPUT_VARIABLE qt5_install_prefix
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(imported_location "${qt5_install_prefix}/bin/windeployqt.exe")

    if(EXISTS ${imported_location})
        add_executable(Qt5::windeployqt IMPORTED)

        set_target_properties(Qt5::windeployqt PROPERTIES
            IMPORTED_LOCATION ${imported_location}
        )
    endif()
endif()

add_custom_command(TARGET ConductorLogFoo
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
        COMMAND set PATH=%PATH%$<SEMICOLON>${qt5_install_prefix}/bin
        COMMAND Qt5::windeployqt --dir "${CMAKE_CURRENT_BINARY_DIR}/windeployqt" "$<TARGET_FILE_DIR:ConductorLogFoo>/$<TARGET_FILE_NAME:ConductorLogFoo>"
    )

# NSIS Packaging
if(WIN32)    
    include(InstallRequiredSystemLibraries)

    # Override the default NSIS template with our own to remove screens we don't want
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/install" ${CMAKE_MODULE_PATH})

    set(CPACK_PACKAGE_INSTALL_DIRECTORY "ConductorLogFoo")
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_VENDOR "ETC Inc")
    set(CPACK_NSIS_MODIFY_PATH "ON")
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY ${PROJECT_NAME})
    set(CPACK_SOURCE_GENERATOR "TGZ")
    set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "${CMAKE_CURRENT_LIST_DIR}/res/Icon.ico")
    set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\ConductorLogFoo.lnk' '$INSTDIR\\\\bin\\\\ConductorLogFoo.exe' ")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_LIST_DIR}/res/Icon.ico")
    set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
    
    install(
        DIRECTORY
        "${CMAKE_CURRENT_BINARY_DIR}/windeployqt/"
        DESTINATION "bin"
    )

    install(
        TARGETS
        ConductorLogFoo
        DESTINATION "bin"
    )

    include(CPack)
endif()