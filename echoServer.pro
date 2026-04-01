QT -= gui
QT += network

CONFIG += c++17 console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

# Исходные файлы сервера
SOURCES += \
    main.cpp \
    mytcpserver.cpp

# Заголовочные файлы сервера
HEADERS += \
    mytcpserver.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
