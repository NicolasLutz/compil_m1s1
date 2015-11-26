TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    symbol.c \
    quad.c

OTHER_FILES += \
    matc.l \
    matc.y \
    Makefile

HEADERS += \
    symbol.h \
    quad.h

