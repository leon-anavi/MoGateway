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

const QString MainWidget::m_sAppName = QString("location2sms");

const char* MainWidget::m_constStrings[] = {
    QT_TRANSLATE_NOOP("MainWidget", "About"),
    QT_TRANSLATE_NOOP("MainWidget", "Language"),
    QT_TRANSLATE_NOOP("MainWidget", "Cancel"),
 };

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    m_pLayout(NULL),
    m_pMainMenu(NULL),
    m_pAboutWidget(NULL),
    m_pLangWidget(NULL),
    m_pMessageBox(NULL),
    m_pOptionsMenu(NULL),
    m_bPortrait(false),
    m_pLocationDataCheckBox(NULL),
    m_pRefreshTimer(NULL)
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
    QString sItemsFont = "font-size: ";
#ifdef Q_OS_SYMBIAN
    sItemsFont += "8pt;";
#else
    sItemsFont += "18pt;";
#endif
    sItemsFont += "font-weight:bold;color: #FFFFFF;";
    m_pLabelHowTo = new QLabel(getHowToText(),this);
    m_pLabelHowTo->setStyleSheet(sItemsFont);
    m_pLabelHowTo->setMinimumHeight(50);
    m_pLabelHowTo->setWordWrap(true);

    m_pLayout = new QVBoxLayout(this);
    m_pLayout->setSpacing(2);
    m_pLayout->setMargin(0);
    m_pLayout->setContentsMargins(5,0,5,0);
    m_pLayout->addWidget(m_pMainMenu, 0, Qt::AlignTop);
    m_pLayout->addWidget(m_pLabelHowTo, 0, Qt::AlignTop);
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

    m_pRefreshTimer = new QTimer(this);
    connect(m_pRefreshTimer, SIGNAL(timeout()), this, SLOT(refresh()));
    m_pRefreshTimer->start(30000);

    m_service = new QMessageService(this);

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
        QMessageFilter::byType(QMessage::Email))); //&
        //QMessageFilter::byStatus(QMessage::Incoming)));

    qDebug() << "Listeing for E-mails";

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
    QMessage message = m_manager->message(m_messageId);

    QMessageAddressList phones = validateEmailAndGetReceivers(message.subject());

    if (0 < phones.count())
    {
        QString sMessageString = "Tel: ";
        for (int nIter=0; nIter < phones.count(); nIter++)
        {
            if (0 < nIter)
            {
                sMessageString += ", ";
            }
            sMessageString += phones.at(nIter).addressee();
        }

        sMessageString += "\nBody: "+ getEmailBody(message);
        m_pLabelHowTo->setText(sMessageString);

        //TODO: send email as SMS

        //Depending the app settings the received email may be removed
        if (true == m_pSettings->isEmailRemovalEnabled())
        {
            m_manager->removeMessage(m_messageId);
            m_service->exportUpdates(QMessageAccount::defaultAccount(QMessage::Email));
        }
    }
}
//------------------------------------------------------------------------------

void MainWidget::createAndShowMessageSent()
{
    m_pMessageBox->clear();

    m_pMessageBox->addLabel(tr("Message sent."));
    m_pMessageBox->addSpacer();
    m_pMessageBox->addButton(tr("OK"));
    m_pMessageBox->setTag(m_nTagMsgSent);

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
    m_pOptionsMenu->addButton(tr(m_constStrings[0]));
    m_pOptionsMenu->addSpacer();
    m_pOptionsMenu->addButton(tr(m_constStrings[2]));
}
//------------------------------------------------------------------------------

QString MainWidget::getEmailBody(QMessage email)
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
    m_service->synchronize(QMessageAccount::defaultAccount(QMessage::Email));
}
//------------------------------------------------------------------------------

QString MainWidget::getHowToText() const
{
    return tr("HowTo");
}
//------------------------------------------------------------------------------

