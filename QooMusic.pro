QT       += core gui sql
QT       += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 resources_big

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += color_octree/ \
               framelesshelper/ \
               loginregister/ \
               win_buttons/

SOURCES += \
    color_octree/coloroctree.cpp \
    color_octree/imageutil.cpp \
    desktoplyricwidget.cpp \
    framelesshelper/CursorPosCalculator.cpp \
    framelesshelper/WidgetData.cpp \
    framelesshelper/frameless_helper.cpp \
    loginregister/bottomlineedit.cpp \
    loginregister/labelededit.cpp \
    loginregister/loginregisterdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    music.cpp \
    musiclist.cpp \
    mymenu.cpp \
    noticedialog.cpp \
    stringutil.cpp \
    tipbox.cpp \
    tipcard.cpp \
    volumecontrol.cpp \
    win_buttons/generalbuttoninterface.cpp \
    win_buttons/interactivebuttonbase.cpp \
    win_buttons/pointmenubutton.cpp \
    win_buttons/threedimenbutton.cpp \
    win_buttons/watercirclebutton.cpp \
    win_buttons/waterfloatbutton.cpp \
    win_buttons/winclosebutton.cpp \
    win_buttons/winmaxbutton.cpp \
    win_buttons/winmenubutton.cpp \
    win_buttons/winminbutton.cpp \
    win_buttons/winrestorebutton.cpp \
    win_buttons/winsidebarbutton.cpp

HEADERS += \
    color_octree/coloroctree.h \
    color_octree/imageutil.h \
    desktoplyricwidget.h \
    framelesshelper/CursorPosCalculator.h \
    framelesshelper/FramelessHelperPrivate.h \
    framelesshelper/WidgetData.h \
    framelesshelper/frameless_helper.h \
    loginregister/bottomlineedit.h \
    loginregister/labelededit.h \
    loginregister/loginregisterdialog.h \
    lyricstreamwidget.h \
    mainwindow.h \
    musicbeans.h \
    musiclist.h \
    mymenu.h \
    netutil.h \
    noticedialog.h \
    notificationentry.h \
    stringutil.h \
    tipbox.h \
    tipcard.h \
    volumecontrol.h \
    win_buttons/generalbuttoninterface.h \
    win_buttons/interactivebuttonbase.h \
    win_buttons/pointmenubutton.h \
    win_buttons/threedimenbutton.h \
    win_buttons/watercirclebutton.h \
    win_buttons/waterfloatbutton.h \
    win_buttons/winclosebutton.h \
    win_buttons/winmaxbutton.h \
    win_buttons/winmenubutton.h \
    win_buttons/winminbutton.h \
    win_buttons/winrestorebutton.h \
    win_buttons/winsidebarbutton.h

FORMS += \
    loginregister/loginregisterdialog.ui \
    mainwindow.ui \
    noticedialog.ui \
    volumecontrol.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon.qrc
