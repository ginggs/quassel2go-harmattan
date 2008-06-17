# Builds the uisupport module

set(QT_DONT_USE_QTGUI 0)
set(QT_USE_QTNETWORK 1)
include(${QT_USE_FILE})

set(SOURCES
    abstractbuffercontainer.cpp
    abstractitemview.cpp
    bufferview.cpp
    bufferviewfilter.cpp
    clearablelineedit.cpp
    colorbutton.cpp
    nickviewfilter.cpp
    inputline.cpp
    nickview.cpp
    settingspage.cpp
    tabcompleter.cpp
    uisettings.cpp
    uistyle.cpp
    uistylesettings.cpp)

set(MOC_HDRS
    abstractbuffercontainer.h
    abstractitemview.h
    bufferview.h
    bufferviewfilter.h
    clearablelineedit.h
    colorbutton.h
    nickviewfilter.h
    inputline.h
    nickview.h
    settingspage.h
    tabcompleter.h)

set(HEADERS
    uisettings.h
    uistyle.h
    uistylesettings.h)

qt4_wrap_cpp(MOC ${MOC_HDRS})

include_directories(${CMAKE_SOURCE_DIR}/src/common
                    ${CMAKE_SOURCE_DIR}/src/client)

add_library(mod_uisupport STATIC ${SOURCES} ${MOC} ${HEADERS})
add_dependencies(mod_uisupport mod_common mod_client)