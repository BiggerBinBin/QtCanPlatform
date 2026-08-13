QT -= gui
QT+= concurrent
TEMPLATE = lib
DEFINES += DEVICEIO_LIBRARY

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    deviceio.cpp \
    peakCAN.cpp

HEADERS += \
    3rd/pcan/include/PCANBasic.h \
    DeviceIO_global.h \
    ReflectClass.h \
    RegisterDevClass.h \
    deviceio.h \
    peakCAN.h

TRANSLATIONS += \
    DeviceIO_zh_CN.ts

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/3rd/pcan/lib/ -lpcanbasic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/3rd/pcan/lib/ -lpcanbasic
else:unix:!macx: LIBS += -L$$PWD/3rd/pcan/lib/ -lpcanbasic

INCLUDEPATH += $$PWD/3rd/pcan/include
#DEPENDPATH += $$PWD/3rd/pcan/lib
