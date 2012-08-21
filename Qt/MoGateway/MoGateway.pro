# Add more folders to ship with the application, here
folder_01.source = qml/TestView
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

symbian:TARGET.UID3 = 0xE28AD74A

# Smart Installer package's UID
# This UID is from the protected range 
# and therefore the package will fail to install if self-signed
# By default qmake uses the unprotected range value if unprotected UID is defined for the application
# and 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# If your application uses the Qt Mobility libraries, uncomment
# the following lines and add the respective components to the 
# MOBILITY variable. 
CONFIG += mobility
MOBILITY = messaging
MOBILITY += systeminfo

symbian:
{
    #TARGET.UID3 = 0x20041ECA
    TARGET.UID3 = 0xE28AD74A
    ICON = MoGateway.svg

    # symbian capabilities
    TARGET.CAPABILITY += NetworkServices \
                            ReadUserData \
                            WriteUserData \
                            ReadDeviceData \
                            WriteDeviceData

    packageheader = "$${LITERAL_HASH}{\"MoGateway\"}, ($${TARGET.UID3}), 1, 0, 1"
    name_deployment.pkg_prerules = packageheader
    DEPLOYMENT += name_deployment

    vendorinfo = "%{\"Leon Anavi\"}" \
        ":\"Leon Anavi\""
    my_deployment.pkg_prerules = vendorinfo
    DEPLOYMENT += my_deployment
}

SOURCES += main.cpp mainwindow.cpp \
    proxywidget.cpp \
    menuwidget.cpp \
    mainwidget.cpp \
    custommessagebox.cpp \
    aboutwidget.cpp \
    settings.cpp \
    languageswidget.cpp \
    settingslistwidget.cpp
HEADERS += mainwindow.h \
    proxywidget.h \
    menuwidget.h \
    mainwidget.h \
    custommessagebox.h \
    aboutwidget.h \
    settings.h \
    languageswidget.h \
    settingslistwidget.h
FORMS +=

TRANSLATIONS = mogateway_bg.ts

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
    qmlapplicationviewer/qmlapplicationviewer.pri \
    qml/TestView/MainPage.qml \
    qml/TestView/main.qml

RESOURCES += \
    MoGatewayResources.qrc
