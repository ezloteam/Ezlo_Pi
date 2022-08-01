#-------------------------------------------------
#
# Project created by QtCreator 2022-02-10T16:01:55
#
#-------------------------------------------------

QT       += core gui serialport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ezlogic
TEMPLATE = app

QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:"level='requireAdministrator'"

SOURCES += main.cpp\
        login.cpp \
        mainwindow.cpp

HEADERS  += mainwindow.h \
    ezuuid.h \
    login.h

#CONFIG += console

FORMS    += mainwindow.ui \
    login.ui

DISTFILES +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=

