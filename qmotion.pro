# -------------------------------------------------
# Project created by QtCreator 2010-02-17T13:21:40
# -------------------------------------------------
QT += network widgets
TARGET = qmotion
TEMPLATE = app
SOURCES += main.cpp \
    recordersetupdlg_impl.cpp \
    recordersetupdlg.cpp \
    qmotion.cpp \
    ftpsetupdlg_impl.cpp \
    ftpsetupdlg.cpp \
    formatconverter.cpp \
    capturethread.cpp \
    motiondetector.cpp \
    mailsender.cpp
HEADERS += recordersetupdlg_impl.h \
    recordersetupdlg.h \
    qmotion.h \
    motiondetector.h \
    ftpsetupdlg_impl.h \
    ftpsetupdlg.h \
    formatconverter.h \
    capturethread.h \
    version.h \
    mailsender.h
FORMS += recordersetupdlg.ui \
    qmotion.ui \
    ftpsetupdlg.ui
RESOURCES += qmotion.qrc
TRANSLATIONS = qmotion_fr.ts
QMAKE_POST_LINK = lrelease \
    qmotion.pro
unix { 
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}
win32 { 
    LIBS += cv.lib \
        highgui.lib \
        cvaux.lib \
        cxcore.lib
    RC_FILE = qmotion.rc
}
OTHER_FILES += 
