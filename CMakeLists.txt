cmake_minimum_required(VERSION 3.14)

project(ElementsExtract VERSION 1.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Strip Release binaries
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

# Qt6 (requerido)
find_package(Qt6 6.2 COMPONENTS Core Widgets Network REQUIRED)
set(QT_VERSION_MAJOR 6)

message(STATUS "Building with Qt ${Qt6_VERSION}")

add_executable(elements-extract
    main.cpp
    mainwindow.cpp
    mainwindow.h
    aboutdialog.cpp
    aboutdialog.h
    helpdialog.cpp
    helpdialog.h
    settingsdialog.cpp
    settingsdialog.h
    subtitleparser.h
    subtitletranslator.h
    subtitletranslator.cpp
    modeldownloader.h
    modeldownloader.cpp
    theme.h
    theme.cpp
    resources.qrc
)

target_link_libraries(elements-extract PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

# --- Instalación ---
install(TARGETS elements-extract DESTINATION bin)
install(FILES ffmpeg-extractor.desktop DESTINATION share/applications PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
install(FILES ffmpeg-extractor.svg DESTINATION share/icons/hicolor/scalable/apps RENAME io.github.pdmiguel90droid.ElementsExtract.svg)

# Install translation engine binary and models (bergamot optional)
if(EXISTS ${CMAKE_SOURCE_DIR}/bin/bergamot)
    install(PROGRAMS ${CMAKE_SOURCE_DIR}/bin/bergamot DESTINATION share/elements-extract/bin)
endif()
if(EXISTS ${CMAKE_SOURCE_DIR}/bin/bergamot.real)
    install(PROGRAMS ${CMAKE_SOURCE_DIR}/bin/bergamot.real DESTINATION share/elements-extract/bin)
endif()
if(EXISTS ${CMAKE_SOURCE_DIR}/bin/libopenblas.so.0)
    install(FILES ${CMAKE_SOURCE_DIR}/bin/libopenblas.so.0 DESTINATION share/elements-extract/bin)
endif()
if(WIN32)
    if(EXISTS ${CMAKE_SOURCE_DIR}/bin/ffmpeg.exe)
        install(PROGRAMS ${CMAKE_SOURCE_DIR}/bin/ffmpeg.exe DESTINATION bin)
    endif()
    if(EXISTS ${CMAKE_SOURCE_DIR}/bin/bergamot.exe)
        install(PROGRAMS ${CMAKE_SOURCE_DIR}/bin/bergamot.exe DESTINATION share/elements-extract/bin)
    endif()
endif()

# Install models without executable bit on .sh files
install(DIRECTORY models/ DESTINATION share/elements-extract/models
        PATTERN "*.sh" EXCLUDE)

# Install translation files
install(DIRECTORY translations/ DESTINATION share/elements-extract/translations
        FILES_MATCHING PATTERN "*.qm")

# Install debian packaging files
if(EXISTS ${CMAKE_SOURCE_DIR}/debian/copyright)
    install(FILES ${CMAKE_SOURCE_DIR}/debian/copyright DESTINATION share/doc/elements-extract)
endif()
if(EXISTS ${CMAKE_SOURCE_DIR}/debian/changelog.gz)
    install(FILES ${CMAKE_SOURCE_DIR}/debian/changelog.gz DESTINATION share/doc/elements-extract)
elseif(EXISTS ${CMAKE_SOURCE_DIR}/debian/changelog)
    install(FILES ${CMAKE_SOURCE_DIR}/debian/changelog DESTINATION share/doc/elements-extract)
endif()

# --- Configuración de CPack para generar .deb ---
set(CPACK_PACKAGE_NAME "elements-extract")
set(CPACK_PACKAGE_VENDOR "pdmiguel90-droid")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Extractor de pistas multimedia con traduccion")
set(CPACK_PACKAGE_DESCRIPTION "Extractor multimedia con traduccion offline de subtitulos")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "pdmiguel90-droid <pdmiguel90-droid@users.noreply.github.com>")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "ffmpeg (>= 4.0), libqt6widgets6 (>= 6.2), libqt6network6 (>= 6.2), libc6 (>= 2.31)")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/pdmiguel90-droid/elements-extract")

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "DEB")
endif()

include(CPack)
