#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

class Settings;

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    void setSettings(const Settings&);
    Settings getSettings(Settings& settings) const;

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
