/*
* ============================================================================
*  Name         : languageswidget.cpp
*  Part of      : mogateway
*  Description  : manage languages
*  Author     	: Leon Anavi
*  Email		: leon@anavi.org
*  License      : GNU General Public License version 3 (GPLv3)
*
*  Copyright (c) 2011-12
* ============================================================================
*/

//Standard includes
#include <QPainter>
#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QFile>

//Project specific includes
#include "languageswidget.h"

LanguagesWidget::LanguagesWidget(Settings* pSettings, QWidget *parent) :
    SettingsListWidget(pSettings, parent)
{
    m_pTitle->setText(getLabelText());
    m_pButtonSelect->setText(getButtonText());

    new QListWidgetItem("English", m_pList);
    new QListWidgetItem(QString::fromUtf8("Български"), m_pList);
}
//------------------------------------------------------------------------------

LanguagesWidget::~LanguagesWidget()
{
    //Nothing to do
}
//------------------------------------------------------------------------------

void LanguagesWidget::select()
{
    int nSelectedLang = m_pList->currentRow();
    if (m_pSettings->getSelectedLanguage() != nSelectedLang)
    {
        m_pSettings->setSelectedLanguage(nSelectedLang);
        loadSelectedLanguage();
    }

    //hide this view
    hide();

    emit settingsWidgetClosed();
}
//------------------------------------------------------------------------------

void LanguagesWidget::loadSelectedLanguage()
{
    //change language
    switch (m_pSettings->getSelectedLanguage())
    {
        case 1:
            //Bulgarian
            loadBulgarian();
        break;
        default:
            //English
            loadEnglish();
        break;
    }
}
//------------------------------------------------------------------------------

void LanguagesWidget::loadBulgarian()
{
    if (m_Translator.load(":/translations/mogateway_bg"))
    {
        qDebug() << "Bulgarian translation loaded.";
    }
    else
    {
        qDebug() << "Unable to load Bulgarian translation.";
    }

    qApp->installTranslator(&m_Translator);
}
//------------------------------------------------------------------------------

void LanguagesWidget::loadEnglish()
{
    qDebug() << "Load English translation.";
    qApp->removeTranslator(&m_Translator);
}
//------------------------------------------------------------------------------

void LanguagesWidget::loadSettings()
{
    m_pList->setCurrentRow(m_pSettings->getSelectedLanguage());
    //by default language is English so there is no need to reload it
    if (0 != m_pSettings->getSelectedLanguage())
    {
        loadSelectedLanguage();
    }
}
//------------------------------------------------------------------------------

void LanguagesWidget::changeEvent(QEvent* event)
{
    if (QEvent::LanguageChange == event->type())
    {
        //translate
        m_pTitle->setText(getLabelText());
        m_pButtonSelect->setText(getButtonText());
    }
    QWidget::changeEvent(event);
}
//------------------------------------------------------------------------------

QString LanguagesWidget::getLabelText() const
{
    return tr("Language:");
}
//------------------------------------------------------------------------------

QString LanguagesWidget::getButtonText() const
{
    return tr("OK");
}
//------------------------------------------------------------------------------
