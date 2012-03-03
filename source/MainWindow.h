#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <boost/circular_buffer.hpp>
#include <kplotobject.h>
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
    KPlotObject* m_plotObject;
    Settings m_settings;
    FILE* m_file;
    QTimer m_timer;
    boost::circular_buffer<unsigned char> m_buffer;
    SampleThread m_sampleThread;
    QMutex m_mutex;

    QMenu* m_fileMenu;
    QMenu* m_toolsMenu;
    QMenu* m_helpMenu;
    //QToolBar* fileToolBar;
    //QToolBar* toolsToolBar;
    QAction* openAct;
    //QAction* saveAct;
    QAction* exitAct;
    QAction* aboutAct;
    QAction* aboutQtAct;

    void createActions();
    void createMenus();
    void initializePlot();

private slots:
    void about();
    void open();
    void plotPoints(const QBitArray& data);
    void sampleAndUpdate();
};

#endif // MAINWINDOW_H
