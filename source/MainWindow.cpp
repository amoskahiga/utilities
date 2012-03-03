#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include "DataPlot.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
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
    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

/**
 * Set up menu items
 */
void MainWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(openAct);
    m_fileMenu->addAction(exitAct);

    menuBar()->addSeparator();

    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(aboutAct);
    m_helpMenu->addAction(aboutQtAct);
}

/**
 * Open the dialog to allow the user to specify the sample file. If a valid file is selected, start sampling it.
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

        m_sampleThread.setFile(m_file);
        m_sampleThread.start();
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
        // Add the points in reserve order to correct data insertion to time order
        m_mutex.lock();
        for (int i = size - 1; i >= 0; --i) {
            tempBuffer[i] = m_buffer[0];
            m_buffer.pop_front();
        }
        m_mutex.unlock();

        // Add the data to our plot
        QBitArray bits = Utility::toBits(tempBuffer, size);
        m_plot->addPoints(bits);
    }
}
