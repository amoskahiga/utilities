#include <QDebug>
#include <qwt5/qwt_painter.h>
#include <qwt5/qwt_plot_canvas.h>
#include <qwt5/qwt_plot_curve.h>
#include <qwt5/qwt_plot_grid.h>
#include <qwt5/qwt_plot_zoomer.h>
#include <qwt5/qwt_plot_panner.h>
#include <qwt5/qwt_scale_widget.h>

#include "DataPlot.h"
#include "Settings.h"

/**
 * Constructor
 */
DataPlot::DataPlot(QWidget *parent) :
    QwtPlot(parent),
    m_curve(new QwtPlotCurve),
    m_zoomer(new QwtPlotZoomer(canvas()))
{
}

/**
 * Setup the GUI plot's settings: axis scale ranges, labels, colors etc
 */
void DataPlot::initialize(const Settings& settings)
{
    alignScales();

    canvas()->setAttribute(Qt::WA_PaintOnScreen, true);

    // Disable polygon clipping
    QwtPainter::setDeviceClipping(false);

    // We don't need the cache here
    canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
    canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, false);

    // Assign a title
    setTitle("Data Plot");

    // Insert new curve
    m_curve->attach(this);

    // Set curve style
    m_curve->setPen(QPen(Qt::red, 1));
    m_curve->setRenderHint(QwtPlotItem::RenderAntialiased);
    m_curve->setStyle(QwtPlotCurve::Steps);

    setSettings(settings);

    // Add grid
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin(true);
    grid->setMajPen(QPen(Qt::white, 0, Qt::DotLine));
    grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
    grid->attach(this);

    // Set up zoomer
    m_zoomer->setZoomBase(true);
    //m_zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
    //m_zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);

    QwtPlotPanner *panner = new QwtPlotPanner(canvas());
    //panner->setAxisEnabled(QwtPlot::yRight, false);
    panner->setMouseButton(Qt::RightButton);

    replot();
}

/**
 * Set a plain canvas frame and align the scales to it
 */
void DataPlot::alignScales()
{
    // The code below shows how to align the scales to
    // the canvas frame, but is also a good example demonstrating
    // why the spreaded API needs polishing.

    canvas()->setFrameStyle(QFrame::Box | QFrame::Plain );
    canvas()->setLineWidth(1);

    for ( int i = 0; i < QwtPlot::axisCnt; i++ )
    {
        QwtScaleWidget *scaleWidget = (QwtScaleWidget *)axisWidget(i);
        if ( scaleWidget )
            scaleWidget->setMargin(0);

        QwtScaleDraw *scaleDraw = (QwtScaleDraw *)axisScaleDraw(i);
        if ( scaleDraw )
            scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    }
}

/**
 * Fill all y-values with the specified value
 *
 * @param value to sey all y-values to: 0 by default
 */
void DataPlot::fill(double value)
{
    m_yPoints.fill(value);
}

void DataPlot::setSettings(const Settings& settings)
{
    // Add axis
    setAxisTitle(QwtPlot::xBottom, "Time (sec)");
    setAxisScale(QwtPlot::xBottom, settings.xMin, settings.xMax);
    setAxisTitle(QwtPlot::yLeft, "Values");
    setAxisScale(QwtPlot::yLeft, settings.yMin, settings.yMax);

    // Add initial data points
    m_xPoints.clear();
    m_yPoints.clear();
    const double interval = 1 / settings.sampleRate;
    for (double i = settings.xMin; i < settings.xMax; i += interval) {
        m_xPoints.append(i);
        m_yPoints.append(0);
    }
    m_curve->setRawData(m_xPoints.data(), m_yPoints.data(), m_xPoints.size());

    // Reset the zoomer
    m_zoomer->setZoomBase(true);

    replot();
}

/**
 * Plot the data bits to the plot/chart. Existing points are shifted foward by the amount of data available.
 *
 * @param data to add; we assume the bits represent values of either 1 or 0, and that the oldest data point is at [0]
 */
void DataPlot::addPoints(const QBitArray& data)
{
    // Advance all the current points by the amount to be copied
    const int pointsToAdd = std::min(data.size(), m_yPoints.size());

    // If we have points to shift, move them
    if (pointsToAdd < m_yPoints.size()) {
        for (int i = m_yPoints.size(); i > pointsToAdd; --i) {
            m_yPoints[i - 1] = m_yPoints[i - pointsToAdd - 1];
        }
    }

    // Add new points to plot
    for (int i = 0; i < pointsToAdd; ++i) {
        m_yPoints[i] = data[i];
    }

    replot();
}

/**
 * Get the current data points displayed in the plot
 *
 * @return x-y points in the displayed window
 */
QVector<QPointF> DataPlot::getDisplayedPoints() const
{
    // Get x-position of current displayed points
    double startPosition = m_zoomer->zoomRect().bottomLeft().x();
    double endPosition = m_zoomer->zoomRect().bottomRight().x();

    if (m_zoomer->zoomStack().size() > 1) { // If we're zoomed in
        startPosition = m_zoomer->zoomRect().bottomLeft().x();
        endPosition = m_zoomer->zoomRect().bottomRight().x();
    }
    else if (m_xPoints.size()) {
        startPosition = m_xPoints[0];
        endPosition = m_xPoints[m_xPoints.size() - 1];

    }

    // Loop through all avaliable points and add them if they are within range
    QVector<QPointF> points;
    for (int i = 0; i < m_xPoints.size(); ++i) {
        double x = m_xPoints.at(i);
        if (x >= startPosition && x <= endPosition)
        {
            double y = m_yPoints.at(i);
            points.append(QPointF(x, y));
        }
    }
    return points;
}
