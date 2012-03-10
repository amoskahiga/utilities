#include <qwt5/qwt_painter.h>
#include <qwt5/qwt_plot_canvas.h>
#include <qwt5/qwt_plot_curve.h>
#include <qwt5/qwt_plot_grid.h>
#include <qwt5/qwt_plot_zoomer.h>
#include <qwt5/qwt_plot_panner.h>
#include <qwt5/qwt_scale_widget.h>

#include "DataPlot.h"
#include "Settings.h"

DataPlot::DataPlot(QWidget *parent) :
    QwtPlot(parent),
    m_curve(new QwtPlotCurve)
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

    QwtPlotZoomer* zoomer = new QwtPlotZoomer(canvas());
    Q_UNUSED(zoomer);
    //zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
    //zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);

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
    const double interval = 1 / ((settings.xMax - settings.xMin) * settings.sampleRate);
    for (double i = settings.xMin; i < settings.xMax; i += interval) {
        m_xPoints.append(i);
        m_yPoints.append(0);
    }
    m_curve->setRawData(m_xPoints.data(), m_yPoints.data(), m_xPoints.size());

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
