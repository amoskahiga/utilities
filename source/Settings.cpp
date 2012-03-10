#include <kconfiggroup.h>
#include <kglobal.h>

#include "Settings.h"

/**
 * Constructor
 */
Settings::Settings() :
    configName("digiplotrc")
{
}

/**
 * Load the default settings form the default user/system location e.g. ~/.kde4/share/config/dolphinrc
 */
void Settings::loadDefault()
{
    if (config.isNull())
    {
        config = KSharedConfig::openConfig(configName);
    }

    KConfigGroup generalGroup(config, "General");
    recentPath = generalGroup.readEntry("recentPath", QString());

    KConfigGroup plotGroup(config, "Plot");
    xMin = plotGroup.readEntry("xMin", -0.01);
    xMax = plotGroup.readEntry("xMax", 1);
    yMin = plotGroup.readEntry("yMin", -0.5);
    yMax = plotGroup.readEntry("yMax", 1.2);
    sampleRate = plotGroup.readEntry("sampleRate", 14400);
}

/**
 * Save the settings to the default user/system location e.g. ~/.kde4/share/config/dolphinrc
 */
void Settings::saveDefault()
{
    if (config.isNull())
    {
        config = KSharedConfig::openConfig(configName);
    }

    KConfigGroup generalGroup(config, "General");
    generalGroup.writeEntry("recentPath", recentPath);

    KConfigGroup plotGroup(config, "Plot");
    plotGroup.writeEntry("xMin", xMin);
    plotGroup.writeEntry("xMax", xMax);
    plotGroup.writeEntry("yMin", yMin);
    plotGroup.writeEntry("yMax", yMax);
    plotGroup.writeEntry("sampleRate", sampleRate);

    generalGroup.config()->sync();
}
