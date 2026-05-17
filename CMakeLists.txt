cmake_minimum_required(VERSION 3.5)

project(FFmpegExtractor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(FFmpegExtractor
    main.cpp
    mainwindow.cpp
    mainwindow.h
    aboutdialog.cpp
    aboutdialog.h
    resources.qrc
)

target_link_libraries(FFmpegExtractor PRIVATE Qt5::Widgets)

# --- Instalación ---
install(TARGETS FFmpegExtractor DESTINATION bin)
install(FILES ffmpeg-extractor.desktop DESTINATION share/applications)
install(FILES ffmpeg-extractor.svg DESTINATION share/icons/hicolor/scalable/apps RENAME io.github.pdmiguel90droid.FFmpegExtractor.svg)

# --- Configuración de CPack para generar .deb ---
set(CPACK_PACKAGE_NAME "ffmpeg-extractor")
set(CPACK_PACKAGE_VENDOR "pdmiguel90-droid")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Extractor de pistas de audio, video y subtitulos usando FFmpeg.")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "pdmiguel90-droid <tu@email.com>")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "ffmpeg, libqt5widgets5, libqt5svg5")

set(CPACK_GENERATOR "DEB")

include(CPack)
