INCLUDEPATH += $$PWD/include

LIBS += -L$$PWD/BIN -lMapDrawInterface

QMAKE_LFLAGS    += -Wl,-rpath,"'\$$ORIGIN'"/lib
QMAKE_LFLAGS    += -Wl,-rpath-link,/usr/lib
LIBS            += -L/usr/lib -lqdmapacces
LIBS            += -L/usr/lib -lmapcomponents
LIBS            += -L/usr/lib -ldl
