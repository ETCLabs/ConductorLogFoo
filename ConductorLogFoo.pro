# ConductorLogFoo Qt Creator Project

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ConductorLogFoo
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
        src/queryrunner.cpp \
        src/calendarrangewidget.cpp

HEADERS  += src/mainwindow.h \
    src/queryrunner.h \
    src/calendarrangewidget.h

FORMS    += ui/mainwindow.ui


isEmpty(TARGET_EXT) {
    win32 {
        TARGET_CUSTOM_EXT = .exe
    }
    macx {
        TARGET_CUSTOM_EXT = .app
    }
} else {
    TARGET_CUSTOM_EXT = $${TARGET_EXT}
}

win32 {
    DEPLOY_COMMAND = $$shell_quote($$system_path($$(QTDIR)/bin/windeployqt))
    DEPLOY_DIR = $${_PRO_FILE_PWD_}/install/deploy
    DEPLOY_TARGET = $${OUT_PWD}/release/$${TARGET}$${TARGET_CUSTOM_EXT}
    DEPLOY_OPT = --dir $${DEPLOY_DIR}
    DEPLOY_CLEANUP = $$QMAKE_COPY $$shell_quote($$system_path($${DEPLOY_TARGET})) $$shell_quote($$system_path($${DEPLOY_DIR}))
}
macx {
    DEPLOY_COMMAND = macdeployqt
    DEPLOY_DIR = $${_PRO_FILE_PWD_}/install/
    PRE_DEPLOY_COMMAND = $${QMAKE_DEL_FILE} $${_PRO_FILE_PWD_}/install/mac/sACNView.dmg
    DEPLOY_TARGET = $${OUT_PWD}/$${TARGET}$${TARGET_CUSTOM_EXT}
    DEPLOY_CLEANUP = $${_PRO_FILE_PWD_}/install/mac/create-dmg --volname "sACNView_Installer" --volicon "$${_PRO_FILE_PWD_}/res/icon.icns"
    DEPLOY_CLEANUP += --background "$${_PRO_FILE_PWD_}/res/mac_install_bg.png" --window-pos 200 120 --window-size 800 400 --icon-size 100 --icon sACNView.app 200 190 --hide-extension sACNView.app --app-drop-link 600 185
    DEPLOY_CLEANUP += $${_PRO_FILE_PWD_}/install/mac/sACNView.dmg $${OUT_PWD}/$${TARGET}$${TARGET_CUSTOM_EXT}
}

CONFIG( release , debug | release) {
    QMAKE_POST_LINK += $${PRE_DEPLOY_COMMAND} $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += $${DEPLOY_COMMAND} $${DEPLOY_TARGET} $${DEPLOY_OPT}
    QMAKE_POST_LINK += $$escape_expand(\\n\\t) $${DEPLOY_CLEANUP}
}
