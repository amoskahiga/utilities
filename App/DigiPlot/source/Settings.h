#ifndef SETTINGS_H
#define SETTINGS_H

#include <ksharedconfig.h>

/**
 * Class to store and load the application's configuration settings
 */
class Settings
{
public:
    QString recentPath;
    double xMin;
    double xMax;
    double yMin;
    double yMax;
    double sampleRate;

    Settings();
    void loadDefault();
    void saveDefault();

private:

    KSharedConfigPtr config;
    const QString configName;
};

#endif // SETTINGS_H
