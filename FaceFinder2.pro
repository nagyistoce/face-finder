QT = gui core opengl
CONFIG += qt \
 warn_on \
 console \
 stl \
 opengl \
 thread \
 release
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = ui/FaceFinder.ui \
 ui/about.ui \
 ui/get_value.ui \
 ui/vinput_settings.ui \
 ui/recognition_settings.ui \
 ui/recognizer_learning.ui \
 ui/add_individual.ui \
 ui/database_settings.ui \
 ui/scanner_settings.ui \
 ui/picture_list_editor.ui \
 ui/recognizer_testing.ui
HEADERS = src/UIFaceFinderWidget.h \
 src/computationthread.h \
 src/visualizationwidget.h \
 src/MicroCounter.h \
 src/Exception.h \
 src/matrix.h \
 src/NeuralNetworks.h \
 src/FaceRecognizer.h \
 src/FaceDatabase.h \
 src/VideoInput.h \
 src/CountedHandle.h \
 src/uiabout.h \
 src/uigetvalue.h \
 src/uivinputsettings.h \
 src/uirecognitionsettings.h \
 src/uirecognizerlearning.h \
 src/uiaddindividual.h \
 src/uidatabasesettings.h \
 src/uiscannersettings.h \
 src/GMArray.h \
 src/videothread.h \
 src/VideoScanner.h \
 src/texteffects.h \
 src/controlthread.h \
 src/qpicturelisteditor.h \
 src/uirecognizertest.h \
 src/Array.h
SOURCES = src/UIFaceFinderWidget.cpp \
 src/main.cpp \
 src/computationthread.cpp \
 src/visualizationwidget.cpp \
 src/matrix.cpp \
 src/NeuralNetworks.cpp \
 src/FaceRecognizer.cpp \
 src/FaceDatabase.cpp \
 src/VideoInput.cpp \
 src/uiabout.cpp \
 src/uigetvalue.cpp \
 src/uivinputsettings.cpp \
 src/uirecognitionsettings.cpp \
 src/uirecognizerlearning.cpp \
 src/uiaddindividual.cpp \
 src/uidatabasesettings.cpp \
 src/uiscannersettings.cpp \
 src/videothread.cpp \
 src/VideoScanner.cpp \
 src/texteffects.cpp \
 src/controlthread.cpp \
 src/qpicturelisteditor.cpp \
 src/uirecognizertest.cpp
RESOURCES += faceFinder.qrc
INCLUDEPATH += /usr/local/include/opencv /usr/local/cuda/include
LIBS += -lcxcore \
 -lcv \
 -lhighgui \
 -lcvaux \
 -lml \
 -lblas \
 -L/usr/local/cuda/lib64 \
 -lcublas \
 -L/usr/local/lib
TEMPLATE = app
TRANSLATIONS += polish.ts
