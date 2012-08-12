/*
* ============================================================================
*  Name         : settings.cpp
*  Part of      : MoGateway
*  Description  : settings back-end
*  Author     	: Leon Anavi
*  Email		: leon@anavi.org
*  License      : GNU General Public License version 3 (GPLv3)
*
*  Copyright (c) 2012
* ============================================================================
*/

//Standard includes
#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QFile>

//Project specific includes
#include "settings.h"

Settings::Settings(QObject *parent) :
    QObject(parent),
    m_bIsAppStartedForFirstTime(true),
    m_nSelectedLanguage(0),
    m_bEmailRemovalEnabled(false),
    m_nEmailReceivedCount(0),
    m_nSmsSentCount(0)
{
#ifdef Q_OS_SYMBIAN
    m_sSettingsFile = QApplication::applicationDirPath();
#else
    m_sSettingsFile ="/home/user";
#endif

    m_sSettingsFile += "/mogateway.ini";

    loadSettings();
}
//------------------------------------------------------------------------------

Settings::~Settings()
{
    saveSettings();
}
//------------------------------------------------------------------------------

void Settings::loadSettings()
{
    //check if file exists
    QFile settingsFile(m_sSettingsFile);
    if (false == settingsFile.exists())
    {
        qDebug() << "file: " << m_sSettingsFile << " not found.";
        return;
    }
    QSettings settings(m_sSettingsFile, QSettings::NativeFormat);
    m_bIsAppStartedForFirstTime =
                         settings.value("IsAppStartedForFirstTime").toBool();

    //load language
    m_nSelectedLanguage = settings.value("SelectedLanguage").toInt();

    //load email removal settings
    m_bEmailRemovalEnabled = settings.value("EmailRemovalEnabled").toBool();

    m_nEmailReceivedCount = settings.value("EmailReceivedCount").toInt();

    m_nSmsSentCount = settings.value("SmsSentCount").toInt();
}
//------------------------------------------------------------------------------

void Settings::saveSettings()
{
    QSettings settings(m_sSettingsFile, QSettings::NativeFormat);
    settings.setValue("IsAppStartedForFirstTime", m_bIsAppStartedForFirstTime);

    //save language
    settings.setValue("SelectedLanguage", m_nSelectedLanguage);

    //save email removal settings
    settings.setValue("EmailRemovalEnabled", m_bEmailRemovalEnabled);

    //save received email count
    settings.setValue("EmailReceivedCount", m_nEmailReceivedCount);

    //save sent sms count
    settings.setValue("SmsSentCount", m_nSmsSentCount);
}
//------------------------------------------------------------------------------

bool Settings::isAppStartedForFirstTime() const
{
    return m_bIsAppStartedForFirstTime;
}
//------------------------------------------------------------------------------

void Settings::setIsAppStartedForFirstTime(bool bIsFirstTime)
{
    m_bIsAppStartedForFirstTime = bIsFirstTime;
}
//------------------------------------------------------------------------------

int Settings::getSelectedLanguage() const
{
    return m_nSelectedLanguage;
}
//------------------------------------------------------------------------------

void Settings::setSelectedLanguage(int nLanguage)
{
    m_nSelectedLanguage = nLanguage;
}
//------------------------------------------------------------------------------

bool Settings::isEmailRemovalEnabled() const
{
    return m_bEmailRemovalEnabled;
}
//------------------------------------------------------------------------------

void Settings::setIsEmailRemovalEnabled(bool bEmailRemovalEnabled)
{
    m_bEmailRemovalEnabled = bEmailRemovalEnabled;
}
//------------------------------------------------------------------------------

int Settings::getEmailReceivedCount() const
{
    return m_nEmailReceivedCount;
}
//------------------------------------------------------------------------------

void Settings::setEmailReceivedCount(int nEmailReceivedCount)
{
    m_nEmailReceivedCount = nEmailReceivedCount;
}
//------------------------------------------------------------------------------

void Settings::incrementEmailReceivedCount(int nCount)
{
    m_nEmailReceivedCount += nCount;
}
//------------------------------------------------------------------------------

int Settings::getSmsSentCount() const
{
    return m_nSmsSentCount;
}
//------------------------------------------------------------------------------

void Settings::setSmsSentCount(int nSmsSentCount)
{
    m_nSmsSentCount = nSmsSentCount;
}
//------------------------------------------------------------------------------

void Settings::incrementSmsSentCount(int nCount)
{
    m_nSmsSentCount += nCount;
}
//------------------------------------------------------------------------------

