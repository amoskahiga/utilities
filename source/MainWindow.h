#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <boost/circular_buffer.hpp>
#include <QDataStream>
#include <QFile>
#include <QMainWindow>
#include <QMutex>
#include <QTimer>

#include "SampleThread.h"
#include "Settings.h"

namespace Ui {
    class MainWindow;
}

class DataPlot;

/**
 * Main DigiPlot application window that contains the GUI plot
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    Settings m_settings;
    FILE* m_file;
    QTimer m_timer;
    boost::circular_buffer<unsigned char> m_buffer;
    SampleThread m_sampleThread;
    QMutex m_mutex;
    DataPlot* m_plot;

    QMenu* m_fileMenu;
    QMenu* m_toolsMenu;
    QMenu* m_helpMenu;
    //QToolBar* fileToolBar;
    //QToolBar* toolsToolBar;
    QAction* m_openAct;
    //QAction* m_saveAct;
    QAction* m_exitAct;
    QAction* m_configureAct;
    QAction* m_aboutAct;
    QAction* m_aboutQtAct;

    void createActions();
    void createMenus();

private slots:
    void about();
    void open();
    void configure();
    void sampleAndUpdate();
};

#endif // MAINWINDOW_H
