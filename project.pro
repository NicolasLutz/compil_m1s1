TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    symbol.c \
    quad.c

OTHER_FILES += \
    matc.y \
    Makefile

HEADERS += \
    symbol.h \
    quad.h \
    matc.l

