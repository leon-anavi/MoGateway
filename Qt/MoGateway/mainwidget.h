/*
* ============================================================================
*  Name         : mainwidget.h
*  Part of      : location2sms
*  Description  : Main screen
*  Author     	: Leon Anavi
*  Email		: leon@anavi.org
*  License      : GNU General Public License version 3 (GPLv3)
*
*  Copyright (c) 2011-12
* ============================================================================
*/

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

//Standard includes
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QGraphicsView>
#include <QTimeLine>
#include <QtCore/qmath.h>
#include <QList>
#include <QPointer>
//Qt Mobility
#include <qmessage.h>
#include <qmessagemanager.h>
#include <qmessageservice.h>
#include <qsysteminfo.h>

//Project specific includes
#include "aboutwidget.h"
#include "menuwidget.h"
#include "languageswidget.h"
#include "settings.h"
#include "custommessagebox.h"

// QtMobility namespace
QTM_USE_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

private:

    Settings* m_pSettings;

    QVBoxLayout* m_pLayout;

    //main menu
    MenuWidget* m_pMainMenu;

    //all subviews
    QList<QWidget*> m_widgetsCtrl;

    //about widget
    AboutWidget* m_pAboutWidget;

    //language settings view
    LanguagesWidget* m_pLangWidget;

    //Custom message
    CustomMessageBox* m_pMessageBox;

    CustomMessageBox* m_pOptionsMenu;

    CustomMessageBox* m_pSettingsMenu;

    QLabel* m_pLabelHowTo;

    QLabel* m_pLabelStatistics;

    bool m_bPortrait;

    QCheckBox* m_pLocationDataCheckBox;

    static const QString m_sAppName;

    static const int m_nTagMsgNoSIM = 1;

    static const char* m_constStrings[];

    QSystemDeviceInfo* m_pSysInfo;

    QPointer<QMessageService> m_service;

    QPointer<QMessageManager> m_manager;

    QMessageManager::NotificationFilterIdSet m_notifFilterSet;

    QMessageId m_messageId;

    QTimer* m_pRefreshTimer;

    QPushButton* m_pButtonControl;

    bool m_bIsGatewayStarted;

public:

    explicit MainWidget(QWidget *parent = 0);

    virtual ~MainWidget();

signals:

public slots:

private slots:

    /**
      * Show or hide help (about) view
      *
      * @return nothing
      */
    void showOptionsMenu();

    /**
      * Show or hide language settings view
      *
      * @return nothing
      */
    void handleLang();

    /**
      * Handle options menu
      *
      * @return nothing
      */
    void handleOptionsMenu();

    void processIncomingEmail();

    // Listening signals from QMessageManager
    void messageAdded(const QMessageId&,
                        const QMessageManager::NotificationFilterIdSet&);

    /**
      * Refresh to check for new incoming messages
      *
      * @return nothing
      */
    void refresh();


    /**
      * Start or stop the gateway
      *
      * @return nothing
      */
    void controlGateway();

    /**
      * Handle message box
      *
      * @return nothing
      */
    void handleMessageBox();

public:

    /**
      * Resize the GUI of the application
      *
      * @param bPortrait
      * @param bMapWidth
      * @param bMapHeight
      *
      * @return nothing
      */
    void resizeGUI(bool bPortrait);

    /**
      * Get message box
      *
      * @return CustomMessageBox
      */
    CustomMessageBox* getMessageBox() const;

protected:

    //overload from QWidget
    void changeEvent(QEvent* event);

private:

    /**
      * Resize about widget
      *
      * @return nothing
      */
    void resizeAboutAndLang();

    /**
      * Adjust visibility of GUI components
      *
      * @param bVisible
      *
      * @return nothing
      */
    void setCtrlVisible(bool bVisible);

    /**
      * round
      *
      * @param nNumber
      * @param nDecimalPoints
      *
      * @return qreal
      */
    inline int round(qreal nNumber, unsigned char nDecimalPoints) const
    {
        return qFloor(nNumber * 10 * nDecimalPoints);
    }

    /**
      * create and show message for sent panic msg
      *
      * @return nothing
      */
    void createAndShowMessageNoSIM();

    /**
      * Show a sub view and close all other subvies
      *
      * @return nothing
      */
    void showWidget(QWidget* pWidget);

    void createOptionsMenu();

    QString getEmailBody(QMessage email);

    /**
      * Remove HTML tags from string
      *
      *@param sTxt
      *
      *@return QString without tags
      */
    QString stripTags(QString sTxt);

    QMessageAddressList validateEmailAndGetReceivers(QString sSubject);

    /**
      * Get user manual text
      *
      * @return text
      */
    QString getHowToText() const;

    /**
      * Get statistics as text for send and received messages
      *
      * @return nothing
      */
    QString getStatistics() const;

    /**
      * Get control button text
      *
      * @return QString
      */
    QString getCtrlButtonText() const;

    /**
      * is SIM card available?
      *
      * @return nothing
      */
    bool isSimCardAvailable() const;

    /**
      * Send SMS to the specified list of recepients
      *
      * @param phonesList
      * @param sTxt
      *
      * @return nothing
      */
    void sendSMS(QMessageAddressList phonesList, QString sTxt);

    /**
      * Reload statistics
      *
      * @return nothing
      */
    void reloadStats();

};

#endif // MAINWIDGET_H
