/*
* ============================================================================
*  Name         : aboutwidget.cpp
*  Part of      : MoGateway
*  Description  : Show about information
*  Author     	: Leon Anavi
*  Email		: leon@anavi.org
*  License      : GNU General Public License version 3 (GPLv3)
*
*  Copyright (c) 2011-12
* ============================================================================
*/

//Project specific includes
#include "aboutwidget.h"

//Standard includes
#include <QPainter>
#include <QDebug>
#include <QEvent>
#include <QApplication>
#include <QDesktopWidget>

AboutWidget::AboutWidget(QWidget *parent) :
    QWidget(parent),
    m_pAboutWidgetLayout(NULL),
    m_pAboutInfo(NULL)
{
    m_pAboutInfo = new QLabel(getInfoText(), this);
    QString sItemsFont = "font-size:";
#ifdef Q_OS_SYMBIAN
    QRect Screen = QApplication::desktop()->screenGeometry();
    if ( ( (640 == Screen.width()) && (480 == Screen.height()) ) ||
       ( (480 == Screen.width()) && (640 == Screen.height()) ) )
    {
        //Nokia E6
        sItemsFont += "4pt;";
    }
    else
    {
        sItemsFont += "5pt;";
    }
#else
    sItemsFont += "12pt;";
#endif
    sItemsFont += "font-weight:bold;color: #FFFFFF;";
    m_pAboutInfo->setStyleSheet(sItemsFont);
    m_pAboutInfo->setWordWrap(true);

    m_pAboutWidgetLayout = new QVBoxLayout(this);
    m_pAboutWidgetLayout->addWidget(m_pAboutInfo, 0, Qt::AlignTop);
    setLayout(m_pAboutWidgetLayout);
}
//------------------------------------------------------------------------------

AboutWidget::~AboutWidget()
{
    //Nothing to do
}
//------------------------------------------------------------------------------

void AboutWidget::paintEvent(QPaintEvent* /*event*/)
{
    QColor backgroundColor(32,32,32,220);
    QPainter customPainter(this);
    customPainter.fillRect(rect(),backgroundColor);
}
//------------------------------------------------------------------------------

void AboutWidget::mousePressEvent(QMouseEvent* /*event*/)
{
    hide();
}
//------------------------------------------------------------------------------

void AboutWidget::changeEvent(QEvent* event)
{
    if (QEvent::LanguageChange == event->type())
    {
        //translate
        m_pAboutInfo->setText(getInfoText());
    }
    QWidget::changeEvent(event);
}
//------------------------------------------------------------------------------

QString AboutWidget::getInfoText() const
{
    QString sText = "<h2><i><span style=\"color: #128406;\">Mo</span><span style=\"color: #006600;\">Gateway</span></i></h2>\n";
    sText += "1.0.2<br />\n";
    sText += QChar(0x00A9);
    sText += "2012 Leon Anavi<br />\n"
        "http://anavi.org/<br />\n<br />\n";
    sText += tr("This is an open source application available under GPLv3 licence at Gitorious: https://gitorious.org/mogateway. ");
    sText += "<br />\n<br />\n";
    sText += tr("Privacy Policy");
    sText += "<br />\n";
    sText += tr("The Application does not collect or transmit personally "
                "identifiable information and does not monitor your "
                "personal use of the Application.");
    return sText;
}
//------------------------------------------------------------------------------


