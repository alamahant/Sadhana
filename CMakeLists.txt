cmake_minimum_required(VERSION 3.19)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Sadhana LANGUAGES CXX)

option(FLATPAK_BUILD "Building for Flatpak" OFF)
if(FLATPAK_BUILD)
    add_compile_definitions(FLATPAK_BUILD)
    message(STATUS "Building for Flatpak")
endif()

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Multimedia PdfWidgets)

qt_add_executable(Sadhana
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    #WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    deitymodule.cpp deitymodule.h gridview.cpp gridview.h modulemanager.cpp modulemanager.h modulesquare.cpp modulesquare.h
    pujaview.cpp pujaview.h
    imagefullscreendialog.cpp imagefullscreendialog.h
    custommodule.h customodule.cpp stage.cpp stage.h
    createmoduledialog.cpp createmoduledialog.h
    readerdialog.cpp readerdialog.h
    tibetancalendar.cpp tibetancalendar.h
    tibetancalendardialog.cpp tibetancalendardialog.h
    journalmanager.cpp journalmanager.h
    journaldialog.cpp journaldialog.h
    constants.cpp constants.h
    resources.qrc
    donationdialog.cpp donationdialog.h
)

target_link_libraries(Sadhana
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Multimedia
        Qt::PdfWidgets
)


if(FLATPAK_BUILD)
    add_custom_command(TARGET Sadhana POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/images"
            "$<TARGET_FILE_DIR:Sadhana>/images"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/files"
            "$<TARGET_FILE_DIR:Sadhana>/files"
        COMMENT "Copying images and files to build directory"
    )
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/files
        DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/images
        DESTINATION ${CMAKE_INSTALL_BINDIR})
else()
    add_custom_command(TARGET Sadhana POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/images"
            "$<TARGET_FILE_DIR:Sadhana>/images"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/files"
            "$<TARGET_FILE_DIR:Sadhana>/files"
        COMMENT "Copying images and files to build directory (Development)"
    )
endif()

include(GNUInstallDirs)
install(TARGETS Sadhana
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(FLATPAK_BUILD)
    # Install desktop file
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/io.github.alamahant.Sadhana.desktop"
            DESTINATION "share/applications")

    # Install metainfo file
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/io.github.alamahant.Sadhana.metainfo.xml"
            DESTINATION "share/metainfo")

    # Install app icon from root directory
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/io.github.alamahant.Sadhana.png"
            DESTINATION "share/icons/hicolor/512x512/apps"
            RENAME "io.github.alamahant.Sadhana.png")

    # Install icons from favicon directory
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/favicon_io/android-chrome-192x192.png"
            DESTINATION "share/icons/hicolor/192x192/apps"
            RENAME "io.github.alamahant.Sadhana.png")

    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/favicon_io/favicon-16x16.png"
            DESTINATION "share/icons/hicolor/16x16/apps"
            RENAME "io.github.alamahant.Sadhana.png")

    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/favicon_io/favicon-32x32.png"
            DESTINATION "share/icons/hicolor/32x32/apps"
            RENAME "io.github.alamahant.Sadhana.png")

    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/favicon_io/apple-touch-icon.png"
            DESTINATION "share/icons/hicolor/180x180/apps"
            RENAME "io.github.alamahant.Sadhana.png")


endif()



qt_finalize_executable(Sadhana)

