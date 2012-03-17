#ifndef DATAPLOT_H
#define DATAPLOT_H

#include <QBitArray>
#include <QVector>
#include <qwt5/qwt_plot.h>

class QwtPlotCurve;
class QwtPlotZoomer;
class Settings;

class DataPlot : public QwtPlot
{
    Q_OBJECT
public:
    explicit DataPlot(QWidget *parent = 0);
    void addPoints(const QBitArray& data);
    QVector<QPointF> getDisplayedPoints() const;
    void initialize(const Settings& settings);
    void fill(double value = 0);
    void setSettings(const Settings& settings);

signals:

public slots:

private:
    QwtPlotCurve* m_curve;
    QwtPlotZoomer* m_zoomer;
    QVector<double> m_xPoints;
    QVector<double> m_yPoints;

    void alignScales();
};

#endif // DATAPLOT_H
