/*
* ============================================================================
*  Name         : mainwidget.cpp
*  Part of      : location2sms
*  Description  : Main screen
*  Author     	: Leon Anavi
*  Email		: leon@anavi.org
*  License      : GNU General Public License version 3 (GPLv3)
*
*  Copyright (c) 2011-12
* ============================================================================
*/

//Standard includes
#include <QUrl>
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QTimer>
#include <QRegExp>

//Project specific includes
#include "mainwidget.h"

const QString MainWidget::m_sAppName = QString("MoGateway");

const char* MainWidget::m_constStrings[] = {
    QT_TRANSLATE_NOOP("MainWidget", "About"),
    QT_TRANSLATE_NOOP("MainWidget", "Language"),
    QT_TRANSLATE_NOOP("MainWidget", "Cancel"),
    QT_TRANSLATE_NOOP("MainWidget", "Clear Statistics"),
 };

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    m_pLayout(NULL),
    m_pMainMenu(NULL),
    m_pAboutWidget(NULL),
    m_pLangWidget(NULL),
    m_pMessageBox(NULL),
    m_pOptionsMenu(NULL),
    m_pLabelHowTo(NULL),
    m_pLabelStatistics(NULL),
    m_bPortrait(false),
    m_pLocationDataCheckBox(NULL),
    m_pSysInfo(NULL),
    m_pRefreshTimer(NULL),
    m_pButtonControl(NULL),
    m_pCheckBoxRemoveEmails(NULL),
    m_bIsGatewayStarted(false),
    m_bEmailAccountNotFound(false)
{
    m_pSettings = new Settings(this);

    QString sStyleBackground = "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #4c4c4c, stop: 0.5 #333333, stop: 1 #202020);";

    //menu
    m_pMainMenu = new MenuWidget(this);
#ifdef Q_OS_SYMBIAN
    m_pMainMenu->setMaximumHeight(40);
#else
    m_pMainMenu->setMaximumHeight(50);
#endif

    //labels
    QString sFontSize = "font-size: ";
#ifdef Q_OS_SYMBIAN
    sFontSize += "6pt;";
#else
    sFontSize += "16pt;";
#endif
    QString sItemsFont = sFontSize + "color: #FFFFFF;";
    m_pLabelHowTo = new QLabel(getHowToText(),this);
    m_pLabelHowTo->setStyleSheet(sItemsFont);
    m_pLabelHowTo->setMinimumHeight(50);
    m_pLabelHowTo->setWordWrap(true);

    m_pLabelStatistics = new QLabel(getStatistics(),this);
    m_pLabelStatistics->setStyleSheet(sItemsFont);
    m_pLabelStatistics->setMinimumHeight(50);
    m_pLabelStatistics->setWordWrap(true);

    QString sButtonBorder = "border-width:0px;border-style:solid;border-radius: 10px 10px / 10px 10px;";

    QString sStyleBackgroundStart = "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #006600, stop: 0.5 #003300, stop: 1 #001100);";
    QString sStyleBackgroundStop = "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #A00000, stop: 0.5 #680000, stop: 1 #280000);";
    QString sButtonFont = sFontSize+"color: #FFFFFF;";

    m_sButtonStartStyle = sButtonFont+"font-weight:bold;"+sStyleBackgroundStart+sButtonBorder;
    m_sButtonStopStyle = sButtonFont+"font-weight:bold;"+sStyleBackgroundStop+sButtonBorder;

    //control button to start or stop the gateway
    m_pButtonControl = new QPushButton(getCtrlButtonText(), this);
    m_pButtonControl->setMinimumHeight(40);
    m_pButtonControl->setStyleSheet(m_sButtonStartStyle);

    m_pCheckBoxRemoveEmails = new QCheckBox(getRmEmailCheckBoxText(), this);
    m_pCheckBoxRemoveEmails->setStyleSheet(sItemsFont);

    m_pCheckBoxRemoveEmails->setChecked(m_pSettings->isEmailRemovalEnabled());

    m_pLayout = new QVBoxLayout(this);
    m_pLayout->setSpacing(20);
    m_pLayout->addWidget(m_pMainMenu, 0, Qt::AlignTop);
    m_pLayout->addWidget(m_pCheckBoxRemoveEmails, 0, Qt::AlignVCenter);
    m_pLayout->addWidget(m_pLabelStatistics, 0, Qt::AlignVCenter);
    m_pLayout->addWidget(m_pLabelHowTo, 1, Qt::AlignTop);
    m_pLayout->addWidget(m_pButtonControl, 0);
    setLayout(m_pLayout);

    //about
    m_pAboutWidget = new AboutWidget(this);
    m_pAboutWidget->hide();

    m_widgetsCtrl << m_pAboutWidget;

    //options menu
    m_pOptionsMenu = new CustomMessageBox(this);
    createOptionsMenu();
    m_pOptionsMenu->hide();

    m_widgetsCtrl << m_pOptionsMenu;

    //languages
    m_pLangWidget = new LanguagesWidget(m_pSettings, this);
    m_pLangWidget->hide();

    m_widgetsCtrl << m_pLangWidget;

    //message box
    m_pMessageBox = new CustomMessageBox(this);
    m_pMessageBox->hide();

    m_widgetsCtrl << m_pMessageBox;

    // Handle options menu
    connect(m_pMainMenu, SIGNAL(showOptionsMenu()),this, SLOT(showOptionsMenu()));

    connect( m_pOptionsMenu, SIGNAL(buttonClicked()),
             this, SLOT(handleOptionsMenu()) );

    //handle message box signal
    connect( m_pMessageBox, SIGNAL(buttonClicked()),
            this, SLOT(handleMessageBox()) );

    //Now when everything is constructed load languages
    m_pLangWidget->loadSettings();

    //handle views
    if (true == m_pSettings->isAppStartedForFirstTime())
    {
        //Make sure that lang selection view will not be shown next time
        m_pSettings->setIsAppStartedForFirstTime(false);
        //Show screen for language selection at start-up
        m_pLangWidget->show();
    }

    m_pSysInfo = new QSystemDeviceInfo(this);

    m_pRefreshTimer = new QTimer(this);
    connect(m_pRefreshTimer, SIGNAL(timeout()), this, SLOT(refresh()));
    m_pRefreshTimer->start(30000);

    m_service = new QMessageService(this);

    createMessageManager();

    // Connect button signal to appropriate slot
    connect(m_pButtonControl, SIGNAL(released()), this, SLOT(controlGateway()));

    connect(m_pCheckBoxRemoveEmails, SIGNAL(stateChanged(int)),
            this,SLOT(checkBoxRemoveEmailStateChanged(int)));
}
//------------------------------------------------------------------------------

MainWidget::~MainWidget()
{

}
//------------------------------------------------------------------------------

void MainWidget::messageAdded(const QMessageId& id,
    const QMessageManager::NotificationFilterIdSet& matchingFilterIds)
{
    // Message added...
    if (matchingFilterIds.contains(m_notifFilterSet))
    {
        // ...and it fits into our filters, lets process it
        m_messageId = id;
        QTimer::singleShot(0, this, SLOT(processIncomingEmail()));
    }
}
//------------------------------------------------------------------------------

void MainWidget::processIncomingEmail()
{
    if (false == m_bIsGatewayStarted)
    {
        //ignore incoming message if the gateway is not started
        return;
    }

    QMessage message = m_manager->message(m_messageId);

    QMessageAddressList phones = validateEmailAndGetReceivers(message.subject());

    if (0 < phones.count())
    {
        QString sBody = getEmailBody();

        //send email as SMS
        sendSMS(phones, sBody);

        //update e-mail counter
        m_pSettings->incrementEmailReceivedCount(1);

        reloadStats();

        //Depending the app settings the received email may be removed
        if (true == m_pSettings->isEmailRemovalEnabled())
        {
            m_manager->removeMessage(m_messageId);
            m_service->exportUpdates(QMessageAccount::defaultAccount(QMessage::Email));
        }
    }
}
//------------------------------------------------------------------------------

void MainWidget::createAndShowMessageNoSIM()
{
    m_pMessageBox->clear();

    m_pMessageBox->addLabel(tr("Please insert and unlock a SIM card."));
    m_pMessageBox->addSpacer();
    m_pMessageBox->addButton(tr("OK"));
    m_pMessageBox->setTag(m_nTagMsgNoSIM);

    showWidget(m_pMessageBox);
}
//------------------------------------------------------------------------------

void MainWidget::createAndShowMessageEmailNotConfigured()
{
    m_pMessageBox->clear();

    m_pMessageBox->addLabel(tr("Please configure an email account."));
    m_pMessageBox->addSpacer();
    m_pMessageBox->addButton(tr("OK"));
    m_pMessageBox->setTag(m_nTagMsgEmailNotConf);

    showWidget(m_pMessageBox);
}
//------------------------------------------------------------------------------

void MainWidget::createAndShowMessageResetStats()
{
    m_pMessageBox->clear();

    m_pMessageBox->addLabel(tr("Are you sure you want to clear statistics?"));
    m_pMessageBox->addSpacer();
    m_pMessageBox->addButton(tr("Yes"));
    m_pMessageBox->addButton(tr("No"));

    m_pMessageBox->setTag(m_nTagMsgResetStats);
    showWidget(m_pMessageBox);
}
//------------------------------------------------------------------------------

void MainWidget::showOptionsMenu()
{
    showWidget(m_pOptionsMenu);
}
//------------------------------------------------------------------------------

void MainWidget::handleLang()
{
    if (NULL == m_pLangWidget)
    {
        return;
    }
    if (m_pLangWidget->isVisible())
    {
        m_pLangWidget->hide();
    }
    else
    {
        //make sure that the About view is hidden
        m_pAboutWidget->hide();
        m_pLangWidget->show();
    }
}
//------------------------------------------------------------------------------

void MainWidget::resizeGUI(bool bPortrait)
{
    resizeAboutAndLang();
    m_bPortrait = bPortrait;
}
//------------------------------------------------------------------------------

void MainWidget::resizeAboutAndLang()
{
    QRect Screen = rect();
    int nSpace = 20;
    int nPosY = nSpace;
    int nWidth = Screen.width()-2*nSpace;
    int nHeight = Screen.height()-nSpace;

    foreach(QWidget* pSubView, m_widgetsCtrl)
    {
        if (NULL != pSubView)
        {
            pSubView->setGeometry(nSpace, nPosY, nWidth, nHeight);
        }
    }
}
//------------------------------------------------------------------------------

void MainWidget::setCtrlVisible(bool /*bVisible*/)
{
    //TODO: show/hide widgets
}
//------------------------------------------------------------------------------

void MainWidget::changeEvent(QEvent* event)
{
    if (QEvent::LanguageChange == event->type())
    {
        //translate
        m_pLabelHowTo->setText(getHowToText());

        m_pLabelStatistics->setText(getStatistics());

        m_pCheckBoxRemoveEmails->setText(getRmEmailCheckBoxText());

        //reload menus
        createOptionsMenu();
    }
    QWidget::changeEvent(event);
}
//------------------------------------------------------------------------------

void MainWidget::handleOptionsMenu()
{
    if (NULL == m_pOptionsMenu)
    {
        return;
    }
    QPushButton* pButton = m_pOptionsMenu->getLastClickedButton();
    //get last clicked button
    if (pButton->text() == tr(m_constStrings[0]))
    {
        //show about
        showWidget(m_pAboutWidget);

    }
    if (pButton->text() == tr(m_constStrings[1]))
    {
        //show settings view for language
        showWidget(m_pLangWidget);
    }
    if (pButton->text() == tr(m_constStrings[3]))
    {
        //TODO: add a confirmation dialog
        createAndShowMessageResetStats();
    }

    m_pOptionsMenu->hide();
}
//------------------------------------------------------------------------------

CustomMessageBox* MainWidget::getMessageBox() const
{
    return m_pMessageBox;
}
//------------------------------------------------------------------------------

void MainWidget::showWidget(QWidget* pWidget)
{
    foreach( QWidget* pView, m_widgetsCtrl )
    {
        pView->setVisible(pView == pWidget);
    }
}
//------------------------------------------------------------------------------

void MainWidget::createOptionsMenu()
{
    m_pOptionsMenu->clear();
    m_pOptionsMenu->addButton(tr(m_constStrings[1]));
    m_pOptionsMenu->addButton(tr(m_constStrings[3]));
    m_pOptionsMenu->addButton(tr(m_constStrings[0]));
    m_pOptionsMenu->addSpacer();
    m_pOptionsMenu->addButton(tr(m_constStrings[2]));
}
//------------------------------------------------------------------------------

QString MainWidget::getEmailBody()
{
    QString sMsgBody = "";
    QMessage message = m_manager->message(m_messageId);

    QMessageContentContainer bodyPart = message.find(message.bodyId());
    if ( (true == bodyPart.isContentAvailable()) &&
         ("text" == bodyPart.contentType()))
    {
        sMsgBody = bodyPart.textContent();
    }
    else if (0 < message.textContent().length())
    {
        sMsgBody = message.textContent();
    }
    //else body cannot be retrieved

    //return body without XML/HTML tags
    return stripTags(sMsgBody);
}
//------------------------------------------------------------------------------

QString MainWidget::stripTags(QString sTxt)
{
    //remove HTML tags using regular expression
    return sTxt.remove( QRegExp( "<(?:div|span|tr|td|br|body|html|tt|a|strong|p)[^>]*>", Qt::CaseInsensitive ) );
}
//------------------------------------------------------------------------------

QMessageAddressList MainWidget::validateEmailAndGetReceivers(QString sSubject)
{
    QMessageAddressList smsList;
    //check prefix
    QString sPrefix = "[email2sms]";
    if (false == sSubject.startsWith(sPrefix))
    {
        return smsList;
    }

    QString sDelimiter = ",";
    //remote prefix and white spaces
    QString sMSISDNs = sSubject.mid(sPrefix.length()).trimmed();
    sMSISDNs.replace(" ","");

    //split and validate each number
    QStringList phones = sMSISDNs.split(sDelimiter);
    for (int nIter=0; nIter < phones.count(); nIter++)
    {
        QString sPhoneRaw = phones.at(nIter);

        //validate phone number
        QString sPhone = "";
        bool bIsPhoneValid = true;
        for (int nChar=0; nChar < sPhoneRaw.length(); nChar++)
        {
            QChar nCurrentChar = sPhoneRaw.at(nChar);
            if ( (false == nCurrentChar.isDigit()) &&
                 ("+" != nCurrentChar) )
            {
                bIsPhoneValid = false;
                break;
            }
            sPhone += nCurrentChar;
        }

        QMessageAddress newPhone = QMessageAddress(
                                        QMessageAddress::Phone, sPhone);

        //add it but ignore duplicates
        if ( (true == bIsPhoneValid) && (-1 == smsList.indexOf(newPhone)) )
        {
            smsList.append(newPhone);
        }
    }
    return smsList;
}
//------------------------------------------------------------------------------

void MainWidget::refresh()
{
#ifndef Q_OS_SYMBIAN
    if (true == m_bIsGatewayStarted)
    {
        m_service->synchronize(QMessageAccount::defaultAccount(QMessage::Email));
    }
#endif
}
//------------------------------------------------------------------------------

QString MainWidget::getHowToText() const
{
    QString sHowTo = "<h4>%1</h4>%2";
    QString sSubTitle1 = tr("E-mail to SMS Gateway");
    QString sContent1 = tr("Press start and after that send an e-mail with prefix [email2sms] and a list of phone numbers separated with comma at the subject. The e-mail body will be send as SMS to each phone number.");
    return sHowTo.arg(sSubTitle1).arg(sContent1);
}
//------------------------------------------------------------------------------

QString MainWidget::getStatistics() const
{
    QString sStatusPane = "<h4>%1</h4>%2";
    QString sTitle = tr("Status");
    QString sColor = "#A00000";
    QString sStatus = tr("Off");
    if (true == m_bIsGatewayStarted)
    {
        sColor = "#006600";
        sStatus = tr("On");
    }

    QString sStatusMsg = QString("%1 <span style=\"color: %2;\">%3</span>").arg(sTitle).arg(sColor).arg(sStatus);
    QString sStats = tr("Received E-mails: %1 Sent SMS: %2").
            arg(m_pSettings->getEmailReceivedCount()).
            arg(m_pSettings->getSmsSentCount());
    return sStatusPane.arg(sStatusMsg).arg(sStats);
}
//------------------------------------------------------------------------------

QString MainWidget::getCtrlButtonText() const
{
    return (false == m_bIsGatewayStarted) ? tr("Start") : tr("Stop");
}
//------------------------------------------------------------------------------

QString MainWidget::getRmEmailCheckBoxText() const
{
    return tr("Delete received e-mail messages");
}
//------------------------------------------------------------------------------

void MainWidget::controlGateway()
{
    if (false == m_bIsGatewayStarted)
    {
        //check is SIM card is present
        if (false == isSimCardAvailable())
        {
            //show error message
            createAndShowMessageNoSIM();
            return;
        }
        //check is e-mail is configured
        if (false == isEmailConfigured())
        {
            //show error message
            createAndShowMessageEmailNotConfigured();
            return;
        }

        m_pButtonControl->setStyleSheet(m_sButtonStopStyle);
    }
    else
    {
        m_pButtonControl->setStyleSheet(m_sButtonStartStyle);
    }

    m_bIsGatewayStarted = !m_bIsGatewayStarted;
    m_pButtonControl->setText(getCtrlButtonText());
    m_pLabelStatistics->setText(getStatistics());
}
//------------------------------------------------------------------------------

void MainWidget::checkBoxRemoveEmailStateChanged(int /*nState*/)
{
    m_pSettings->setIsEmailRemovalEnabled(m_pCheckBoxRemoveEmails->isChecked());
}
//------------------------------------------------------------------------------

bool MainWidget::isSimCardAvailable() const
{
    QSystemDeviceInfo::SimStatus nStatus = m_pSysInfo->simStatus();
    if ( (QSystemDeviceInfo::SimNotAvailable == nStatus) ||
         (QSystemDeviceInfo::SimLocked == nStatus) )
    {
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------

bool MainWidget::isEmailConfigured()
{
    if (true == m_bEmailAccountNotFound)
    {
        if (NULL != m_manager)
        {
            delete m_manager;
            m_manager = NULL;
        }
        m_notifFilterSet.clear();
        createMessageManager();
    }

    foreach (const QMessageAccountId &id, m_manager->queryAccounts())
    {
        QMessageAccount account(id);
        if (QMessage::Email == account.messageTypes())
        {
            m_bEmailAccountNotFound = false;
            //E-mail account found
            return true;
        }

    }
    m_bEmailAccountNotFound = true;
    return false;
}
//------------------------------------------------------------------------------

void MainWidget::handleMessageBox()
{
    if (NULL == m_pMessageBox)
    {
        return;
    }

    if (m_pMessageBox->getTag() == m_nTagMsgResetStats)
    {
        QPushButton* pButton = m_pMessageBox->getLastClickedButton();
        //get last clicked button
        if (pButton->text() == tr("Yes"))
        {
            //clear statistics
            resetStats();
        }
    }

    //just hide the message box
    m_pMessageBox->hide();
}
//------------------------------------------------------------------------------

void MainWidget::sendSMS(QMessageAddressList phonesList, QString sTxt)
{
    // Create  a new email
    QMessage msg;
    msg.setType(QMessage::Sms);
    // Add required parameters
    msg.setTo(phonesList);
    msg.setSubject(m_sAppName);
    //Set message body
    msg.setBody(sTxt);
    //TODO: Send SMS
    //m_service->send(msg);

    //update counters
    m_pSettings->incrementSmsSentCount(phonesList.count());
}
//------------------------------------------------------------------------------

void MainWidget::reloadStats()
{
    m_pLabelStatistics->setText(getStatistics());
}
//------------------------------------------------------------------------------

void MainWidget::resetStats()
{
    m_pSettings->setSmsSentCount(0);
    m_pSettings->setEmailReceivedCount(0);
    reloadStats();
}
//------------------------------------------------------------------------------

void MainWidget::createMessageManager()
{
    // Manager for listening messages
    m_manager = new QMessageManager(this);

    // Listen new added messages
    connect(m_manager, SIGNAL(messageAdded(const QMessageId&,
                              const QMessageManager::NotificationFilterIdSet&)),
            this, SLOT(messageAdded(const QMessageId&,
                       const QMessageManager::NotificationFilterIdSet&)));

    // Create 2 filers set for filtering messages
    // - SMS filter
    // - InboxFolder filter
    m_notifFilterSet.insert(m_manager->registerNotificationFilter(
        QMessageFilter::byType(QMessage::Email) &
        QMessageFilter::byStatus(QMessage::Incoming)));
}
//------------------------------------------------------------------------------
