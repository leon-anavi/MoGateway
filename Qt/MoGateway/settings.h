/*
* ============================================================================
*  Name         : settings.h
*  Part of      : location2sms
*  Description  : settings back-end
*  Author     	: Leon Anavi
*  Email		: leon@anavi.org
*  License      : GNU General Public License version 3 (GPLv3)
*
*  Copyright (c) 2012
* ============================================================================
*/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

class Settings : public QObject
{
    Q_OBJECT

public:

    enum MapTypes { google, bing, nokia, openstreetmap };

private:

    QString m_sSettingsFile;

    bool m_bIsAppStartedForFirstTime;

    int m_nSelectedLanguage;

    bool m_bEmailRemovalEnabled;

public:

    explicit Settings(QObject *parent = 0);

    virtual ~Settings();

    /**
      * Is application started for first time
      *
      * @return bool
      */
    bool isAppStartedForFirstTime() const;

    /**
      * set if the application is started for first time or not
      *
      * @param bIsFirstTime
      *
      * @return noting
      */
    void setIsAppStartedForFirstTime(bool bIsFirstTime);

    /**
      * Get selected language
      *
      * @return int
      */
    int getSelectedLanguage() const;

    /**
      * set selected language
      *
      * @param nLanguage
      *
      * @return nothing
      */
    void setSelectedLanguage(int nLanguage);

    /**
      * Is removal of emails enabled
      *
      * @return bool
      */
    bool isEmailRemovalEnabled() const;

    /**
      * Enable or disable removal of emails
      *
      * @param bEmailRemovalEnabled
      *
      * @return nothing
      */
    void setIsEmailRemovalEnabled(bool bEmailRemovalEnabled);

private:

    /**
      * Read settings and load appropriate language
      *
      * @return nothing
      */
    void loadSettings();

    /**
      * Save settings
      *
      * @return nothing
      */
    void saveSettings();

signals:
    
public slots:
    
};

#endif // SETTINGS_H
