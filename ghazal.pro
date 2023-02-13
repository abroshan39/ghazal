QT     += core gui widgets sql concurrent xml network

greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

OUTPUT_FILE_NAME_UNIX = "ghazal"
OUTPUT_FILE_NAME_WIN = "Ghazal"
OUTPUT_FILE_NAME_MAC = "Ghazal"
QUAZIP_LIB_NAME_UNIX = "quazip1-qt5"
QUAZIP_LIB_NAME_WIN = "quazip1-qt5"
QUAZIP_LIB_NAME_MAC = "quazip1-qt5"

unix:!mac {
    TARGET = $$OUTPUT_FILE_NAME_UNIX
    LIBS += -L$$PWD/libraries/unix/quazip -l$$QUAZIP_LIB_NAME_UNIX
}

win32 {
    TARGET = $$OUTPUT_FILE_NAME_WIN
    LIBS += -L$$PWD/libraries/win/quazip -l$$QUAZIP_LIB_NAME_WIN
    RC_FILE = $$PWD/resources/resource_win.rc
}

mac {
    TARGET = $$OUTPUT_FILE_NAME_MAC
    LIBS += -L$$PWD/libraries/mac/quazip -l$$QUAZIP_LIB_NAME_MAC
}

INCLUDEPATH += $$PWD/libraries/include

HEADERS += \
    src/abjad_class.h \
    src/abjadform.h \
    src/abjadformmini.h \
    src/aboutauthorform.h \
    src/aboutform.h \
    src/appthemes.h \
    src/common_functions.h \
    src/databaseform.h \
    src/date_converter.h \
    src/downloadform.h \
    src/event_functions.h \
    src/filedownloader.h \
    src/mainwindow.h \
    src/searchexamplesform.h \
    src/searchform.h \
    src/settingsform.h \
    src/tabform.h \
    src/updatecheckform.h \
    src/version.h \
    src/wordsearchform.h \
    src/worker.h

SOURCES += \
    src/abjad_class.cpp \
    src/abjadform.cpp \
    src/abjadformmini.cpp \
    src/aboutauthorform.cpp \
    src/aboutform.cpp \
    src/appthemes.cpp \
    src/common_functions.cpp \
    src/common_search.cpp \
    src/databaseform.cpp \
    src/date_converter.c \
    src/downloadform.cpp \
    src/event_functions.cpp \
    src/filedownloader.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/mainwindow_action_menu.cpp \
    src/mainwindow_app_setting.cpp \
    src/mainwindow_search_form.cpp \
    src/searchexamplesform.cpp \
    src/searchform.cpp \
    src/settingsform.cpp \
    src/tabform.cpp \
    src/tabform_context_menu.cpp \
    src/updatecheckform.cpp \
    src/wordsearchform.cpp \
    src/worker.cpp

FORMS += \
    src/abjadform.ui \
    src/abjadformmini.ui \
    src/aboutauthorform.ui \
    src/aboutform.ui \
    src/databaseform.ui \
    src/downloadform.ui \
    src/mainwindow.ui \
    src/searchexamplesform.ui \
    src/searchform.ui \
    src/settingsform.ui \
    src/tabform.ui \
    src/updatecheckform.ui \
    src/wordsearchform.ui

RESOURCES += \
    resources/themes/darkstyle.qrc \
    resources/resource.qrc
