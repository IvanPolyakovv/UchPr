QT -= gui
QT += core
QT += sql
CONFIG += console c++17
CONFIG -= app_bundle

TEMPLATE = app
TARGET = Proj

SOURCES += main.cpp \
    ahp.cpp \
    apartment.cpp \
    database.cpp \
    utils.cpp \
    webui.cpp

INCLUDEPATH += $$PWD/crow
INCLUDEPATH += $$PWD/asio

LIBS += -lws2_32 -lmswsock
DEFINES += _WIN32_WINNT=0x0601

# Если crow будет где-то в другом месте, например /thirdparty/crow, тогда:
# INCLUDEPATH += $$PWD/thirdparty/crow

# Путь установки (можно не трогать пока)
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ahp.h \
    apartment.h \
    database.h \
    utils.h \
    webui.h
