#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include "DataPlot.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "SettingsDialog.h"
#include "Utility.h"

/**
 * Constructor
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_file(0),
    m_sampleThread(m_buffer, m_mutex, this),
    m_plot(0)
{
    ui->setupUi(this);
    createActions();
    createMenus();

    m_settings.loadDefault();

    m_plot = new DataPlot(this);
    setCentralWidget(m_plot);
    m_plot->setMinimumSize(400, 400);
    m_plot->initialize(m_settings);

    m_timer.setInterval(100);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(sampleAndUpdate()));
    m_timer.start();

    m_buffer.set_capacity(1000000);
}

/**
 * Destructor
 */
MainWindow::~MainWindow()
{
    if (m_file) {
        m_sampleThread.stop();
        m_sampleThread.wait();
        fclose(m_file);
    }
    m_file = 0;
    m_settings.saveDefault();

    delete ui;
}

/**
 * Display the about dialog
 */
void MainWindow::createActions()
{
    m_openAct = new QAction(QIcon(":/images/open.png"), "&Open...", this);
    m_openAct->setShortcuts(QKeySequence::Open);
    m_openAct->setStatusTip(tr("Open an existing file"));
    connect(m_openAct, SIGNAL(triggered()), this, SLOT(open()));

    m_saveAct = new QAction("&Save", this);
    m_saveAct->setShortcuts(QKeySequence::Save);
    m_saveAct->setStatusTip(tr("Save to current file"));
    connect(m_saveAct, SIGNAL(triggered()), this, SLOT(save()));

    m_saveAsAct = new QAction("&Save As...", this);
    m_saveAsAct->setShortcuts(QKeySequence::SaveAs);
    m_saveAsAct->setStatusTip(tr("Save to specified file"));
    connect(m_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    m_configureAct = new QAction("&Configure DigiPlot...", this);
    m_configureAct->setStatusTip(tr("Configure the application's settings"));
    connect(m_configureAct, SIGNAL(triggered()), this, SLOT(configure()));

    m_aboutAct = new QAction("&About", this);
    m_aboutAct->setStatusTip("Show the application's About box");
    connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    m_aboutQtAct = new QAction("About &Qt", this);
    m_aboutQtAct->setStatusTip("Show the Qt library's About box");
    connect(m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    m_exitAct = new QAction("E&xit", this);
    m_exitAct->setShortcuts(QKeySequence::Quit);
    m_exitAct->setStatusTip("Exit the application");
    connect(m_exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

/**
 * Set up menu items
 */
void MainWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_openAct);
    m_fileMenu->addAction(m_saveAct);
    m_fileMenu->addAction(m_saveAsAct);
    m_fileMenu->addAction(m_exitAct);

    m_toolsMenu = menuBar()->addMenu(tr("&Settings"));
    m_toolsMenu->addAction(m_configureAct);

    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_aboutAct);
    m_helpMenu->addAction(m_aboutQtAct);
}

/**
 * Open a dialog to allow the user to specify the sample file. If a valid file is selected, start sampling it.
 */
void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", m_settings.recentPath);
    if (!fileName.isEmpty()) {

        // Close any active file
        if (m_file) {
            m_sampleThread.stop();
            m_sampleThread.wait();
            fclose(m_file);
        }

        if ((m_file = fopen(fileName.toStdString().c_str(), "rb")) == NULL) {
            QMessageBox::warning(this, "Application", "Cannot read file: " + fileName);
            return;
        }
        statusBar()->showMessage("File opened", 2000);
        QFileInfo fileInfo(fileName);
        m_settings.recentPath = fileInfo.filePath();

        // Reset the current view
        m_plot->setSettings(m_settings);

        m_sampleThread.setFile(m_file);
        m_sampleThread.start();
    }
}

/**
 * Open a dialog to allow the user to specify a file to save the currently displayed samples to. If a valid file is
 * selected, save the samples to it.
 */
void MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Open File", m_settings.recentPath);
    if (!fileName.isEmpty()) {

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::warning(this, "Application", "Cannot read file: " + fileName);
            return;
        }

        QVector<QPointF> points = m_plot->getDisplayedPoints();
        QVector<double> yPoints = Utility::getYValues(points);
        QBitArray bits = Utility::toBits(yPoints);
        QVector<char> chars = Utility::toChars(bits);

        // Save the chars to file
        QDataStream outStream(&file);
        outStream.writeRawData(chars.data(), chars.size());

        statusBar()->showMessage("File saved", 2000);
    }
}

/**
 * Open the configuration dialog with the current settings, allowing the user to modify and persist them
 */
void MainWindow::configure()
{
    SettingsDialog settingsDialog;
    settingsDialog.setSettings(m_settings);
    if (settingsDialog.exec() == QDialog::Accepted) {
        settingsDialog.getSettings(m_settings);
        m_plot->setSettings(m_settings);
    }
}


/**
 * Display the about dialog
 */
void MainWindow::about()
{
   QMessageBox::about(this, tr("About DigiPlot"),
            tr("The <b>DigiPlot</b> provides a graphing interface to plot real-time or archived data."));
}

/**
 * Read the available data from the shared buffer and plot it. Access to the buffer is synchronized with a mutex.
 */
void MainWindow::sampleAndUpdate()
{
    // If we have an open file and some data to read
    if (m_file && m_buffer.size()) {
        // Get data from the buffer
        const size_t size = m_buffer.size();
        // char* buffer = m_buffer.linearize();
        char tempBuffer[size];
        m_mutex.lock();
        for (size_t i = 0; i < size; ++i) {
            tempBuffer[i] = m_buffer[0];
            m_buffer.pop_front();
        }
        m_mutex.unlock();

        // Add the data to our plot
        QBitArray bits = Utility::toBits(tempBuffer, size);
        m_plot->addPoints(bits);
    }
}
