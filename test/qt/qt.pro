TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../../lib/lagan-clang/lagan.c \
        main.c \
    ../../async.c \
    ../../lib/tzlist/tzlist.c \
    ../../lib/tzmalloc/bget.c \
    ../../lib/tzmalloc/tzmalloc.c \
    ../../lib/tztime/tztime.c \

INCLUDEPATH += ../.. \
    ../../lib/tzlist \
    ../../lib/tzmalloc \
    ../../lib/tztime \
    ../../lib/pt \
    ../../lib/lagan-clang

HEADERS += \
    ../../async.h \
    ../../lib/lagan-clang/lagan.h \
    ../../lib/pt/lc.h \
    ../../lib/pt/lc-switch.h \
    ../../lib/pt/pt.h \
    ../../lib/pt/pt-sem.h \
    ../../lib/tzlist/tzlist.h \
    ../../lib/tzmalloc/bget.h \
    ../../lib/tzmalloc/tzmalloc.h \
    ../../lib/tztime/tztime.h
