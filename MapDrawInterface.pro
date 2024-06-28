QT += core widgets
CONFIG += c++14 console

INCLUDEPATH += src
TEMPLATE = lib
TARGET = MapDrawInterface

include(src/src.pri)
include(build.pri)
include(deps.pri)
