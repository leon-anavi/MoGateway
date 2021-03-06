/*
* ============================================================================
*  Name         : mainwidget.h
*  Part of      : MoGateway
*  Description  : Main screen
*  Author     	: Leon Anavi
*  Email		: leon@anavi.org
*  License      : GNU General Public License version 3 (GPLv3)
*
*  Copyright (c) 2012
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

    static const int m_nTagMsgEmailNotConf = 2;

    static const int m_nTagMsgResetStats = 3;

    static const char* m_constStrings[];

    QSystemDeviceInfo* m_pSysInfo;

    QPointer<QMessageService> m_service;

    QPointer<QMessageManager> m_manager;

    QMessageManager::NotificationFilterIdSet m_notifFilterSet;

    QMessageId m_messageId;

    QTimer* m_pRefreshTimer;

    QPushButton* m_pButtonControl;

    QCheckBox* m_pCheckBoxRemoveEmails;

    bool m_bIsGatewayStarted;

    bool m_bEmailAccountNotFound;

    QString m_sButtonStartStyle;

    QString m_sButtonStopStyle;

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
      * change state of the checkbox for removing e-mails
      *
      * @param int
      *
      * @return nothing
      */
    void checkBoxRemoveEmailStateChanged(int nState);

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
      * create and show error message when SIM is not available
      *
      * @return nothing
      */
    void createAndShowMessageNoSIM();

    /**
      * create and show error message when e-mail is not configured
      *
      * @return nothing
      */
    void createAndShowMessageEmailNotConfigured();

    /**
      * create and show a message to confirm reset of statistics
      *
      * @return nothing
      */
    void createAndShowMessageResetStats();

    /**
      * Show a sub view and close all other subvies
      *
      * @return nothing
      */
    void showWidget(QWidget* pWidget);

    void createOptionsMenu();

    QString getEmailBody();

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
      * get text for checkbox for remove received emails
      *
      * @return QString
      */
    QString getRmEmailCheckBoxText() const;

    /**
      * is SIM card available?
      *
      * @return nothing
      */
    bool isSimCardAvailable() const;

    /**
      * Is e-mail account configured for the whole OS
      *
      *@return bool
      */
    bool isEmailConfigured();

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

    /**
      * Reset statistics
      *
      * @return nothing
      */
    void resetStats();

    /**
      * create message manager and start listening for incoming emails
      *
      * @return nothing
      */
    void createMessageManager();

};

#endif // MAINWIDGET_H
