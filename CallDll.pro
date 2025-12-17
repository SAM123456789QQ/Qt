QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DESTDIR = $$PWD/APP

# 应用名称
TARGET  = LzTools

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
    commonLibrary/openCV/facerecognitionmanager.cpp \
    global.cpp \
    main.cpp \
    mainwindow.cpp \
    matlabCallDll/form_matlabdll.cpp \
    matlabCallDll/mythread_e.cpp \
    sqlits3DateBase/databasemanager.cpp

HEADERS += \
    commonLibrary/openCV/facerecognitionmanager.h \
    global.h \
    mainwindow.h \
    matlabCallDll/form_matlabdll.h \
    matlabCallDll/mythread_e.h \
    sqlits3DateBase/databasemanager.h

FORMS += \
    mainwindow.ui \
    matlabCallDll/form_matlabdll.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# MatLab 环境
INCLUDEPATH += $$quote(C:/Program Files/MATLAB/R2024a/extern/include)

INCLUDEPATH += $$quote(C:/Program Files/MATLAB/R2024a/extern/lib/win64/microsoft)
DEPENDPATH += $$quote(C:/Program Files/MATLAB/R2024a/extern/lib/win64/microsoft)
win32: LIBS += -L$$quote(C:/Program Files/MATLAB/R2024a/extern/lib/win64/microsoft/) -llibeng
win32: LIBS += -L$$quote(C:/Program Files/MATLAB/R2024a/extern/lib/win64/microsoft/) -llibmx
win32: LIBS += -L$$quote(C:/Program Files/MATLAB/R2024a/extern/lib/win64/microsoft/) -llibmat
win32: LIBS += -L$$quote(C:/Program Files/MATLAB/R2024a/extern/lib/win64/microsoft/) -llibmex
win32: LIBS += -L$$quote(C:/Program Files/MATLAB/R2024a/extern/lib/win64/microsoft/) -lmclmcr
win32: LIBS += -L$$quote(C:/Program Files/MATLAB/R2024a/extern/lib/win64/microsoft/) -lmclmcrrt

DEFINES +=__MW_STDINT_H__

# 添加 matlab 打包的动态链接库
INCLUDEPATH += $$PWD/matlabCallDll/include
DEPENDPATH += $$PWD/matlabCallDll/include

win32:LIBS += -L$$PWD/matlabCallDll/include/ -lcomplex_matlab_func
win32:LIBS += -L$$PWD/matlabCallDll/include/ -lread_mat_file

# openCV 环境
INCLUDEPATH += E:\Technology\OpenCV\opencv\opencv-build\install\include
LIBS += E:\Technology\OpenCV\opencv\opencv-build\install\x64\mingw\lib\libopencv_*.a

RESOURCES += \
    resource.qrc

RC_FILE = myicon.rc
