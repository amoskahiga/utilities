#include <kplotaxis.h>
#include <kplotobject.h>
#include <kplotpoint.h>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Utility.h"

/**
 * Constructor
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_plotObject(0),
    m_file(0),
    m_sampleThread(m_buffer, m_mutex, this)
{
    ui->setupUi(this);
    createActions();
    createMenus();

    m_settings.loadDefault();
    initializePlot();

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
 * Setup the GUI plot's settings: axis scale ranges, labels, colors etc
 */
void MainWindow::initializePlot()
{
    // Create a new KPlotWidget, provide a minimum size, and activate antialiased drawing:
    ui->kplotwidget->setMinimumSize(400, 400);
    ui->kplotwidget->setAntialiasing( true );

    // Set the boundaries of the plot to display the full extent of the sine curve:
    ui->kplotwidget->setLimits(m_settings.xMin, m_settings.xMax, m_settings.yMin, m_settings.yMax);

    // Create a KPlotObject to display data points by connecting them with red line segments and a width of 2 pixels
    m_plotObject = new KPlotObject(Qt::red, KPlotObject::Lines, 2);

    // Add initial data points to the KPlotObject
    const double interval = 1 / ((m_settings.xMax - m_settings.xMin) * m_settings.sampleRate);
    for (double i = m_settings.xMin; i < m_settings.xMax; i += interval) {
        m_plotObject->addPoint(i, 0);
    }

    // Add the KPlotObject to the plot:
    ui->kplotwidget->addPlotObject(m_plotObject);

    // Add a text label to the bottom axis:
    ui->kplotwidget->axis(KPlotWidget::BottomAxis)->setLabel("Time (sec)");
    ui->kplotwidget->axis(KPlotWidget::LeftAxis)->setLabel("Magnitude");

    //ui->kplotwidget->setBackgroundColor(QColor("white"));
    //ui->kplotwidget->setForegroundColor(QColor("black"));
    //ui->kplotwidget->setGridColor(QColor("black"));

    ui->kplotwidget->setShowGrid(true);

    ui->kplotwidget->update();
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
        plotPoints(bits);

        ui->kplotwidget->update();  // Refresh the plot
    }
}

/**
 * Plot the data bits to the plot/chart. Existing points are shifted foward by the amount of data available.
 *
 * @param data to add; we assume the bits represent values of either 1 or 0, and that the oldest data point is at [0]
 */
void MainWindow::plotPoints(const QBitArray& data)
{
    // Advance all the current points by the amount to be copied
    QList<KPlotPoint*> points = m_plotObject->points();
    const int pointsToAdd = std::min(data.size(), points.size());

    // If we have points to shift, move them
    if (pointsToAdd < points.size()) {
        for (int i = points.size(); i > pointsToAdd; --i) {
            KPlotPoint* newPoint = points[i - 1];
            KPlotPoint* oldPoint = points[i - pointsToAdd - 1];
            newPoint->setY(oldPoint->y());
        }
    }

    // Add new points to plot
    for (int i = 0; i < pointsToAdd; ++i) {
        points[i]->setY(data[i]);
    }
}
