include(./qwt.pri)

RESOURCES += $$PWD/qml.qrc

HEADERS += \
    $$PWD/datamodel.h \
    $$PWD/launcher.h \
    $$PWD/fileutils.h \
    $$PWD/qtavplayerutils.h \
    $$PWD/qwtquick2plot.h \
    $$PWD/xmlparser.h \
    $$PWD/plotdata.h \
    $$PWD/sortfiltertablemodel.h

SOURCES += \
    $$PWD/datamodel.cpp \
    $$PWD/launcher.cpp \
    $$PWD/fileutils.cpp \
    $$PWD/qtavplayerutils.cpp \
    $$PWD/qwtquick2plot.cpp \
    $$PWD/xmlparser.cpp \
    $$PWD/plotdata.cpp \
    $$PWD/sortfiltertablemodel.cpp

INCLUDEPATH += $$PWD

DISTFILES += \
    $$PWD/qwt.pri \
    icons/fastforward.svg \
    icons/play.svg \
    icons/rewind.svg \
    icons/stop.svg
