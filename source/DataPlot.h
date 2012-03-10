#ifndef DATAPLOT_H
#define DATAPLOT_H

#include <QBitArray>
#include <QVector>
#include <qwt5/qwt_plot.h>

class QwtPlotCurve;
class Settings;

class DataPlot : public QwtPlot
{
    Q_OBJECT
public:
    explicit DataPlot(QWidget *parent = 0);
    void initialize(const Settings& settings);
    void addPoints(const QBitArray& data);
    void setSettings(const Settings& settings);

signals:

public slots:

private:
    QwtPlotCurve* m_curve;
    QVector<double> m_xPoints;
    QVector<double> m_yPoints;

    void alignScales();
};

#endif // DATAPLOT_H
