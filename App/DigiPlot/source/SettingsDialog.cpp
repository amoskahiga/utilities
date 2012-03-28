#include "Settings.h"
#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::setSettings(const Settings& settings)
{
    ui->sampleRateDoubleSpinBox->setValue(settings.sampleRate);
    ui->xMinimumDoubleSpinBox->setValue(settings.xMin);
    ui->xMaximumDoubleSpinBox->setValue(settings.xMax);
    ui->yMinimumDoubleSpinBox->setValue(settings.yMin);
    ui->yMaximumDoubleSpinBox->setValue(settings.yMax);
}

Settings SettingsDialog::getSettings(Settings& settings) const
{
    settings.sampleRate = ui->sampleRateDoubleSpinBox->value();
    settings.xMin = ui->xMinimumDoubleSpinBox->value();
    settings.xMax = ui->xMaximumDoubleSpinBox->value();
    settings.yMin = ui->yMinimumDoubleSpinBox->value();
    settings.yMax = ui->yMaximumDoubleSpinBox->value();

    return settings;
}
