#include "osc_2nd_deg_free.h"
#include "ui_osc_2nd_deg_free.h"

using namespace o2df;

Osc2ndDegFree::Osc2ndDegFree(QWidget *parent) : TaskWindow(parent), ui(new Ui::Osc2ndDegFree)
{
    ui->setupUi(this);
    initUI();
}

Osc2ndDegFree::~Osc2ndDegFree()
{
    delete ui;
}

void Osc2ndDegFree::redrawPlots()
{
    clearPlots();
    setParametersFromUI();
    
    double t = 0;
    double x1 = param.x1, x2 = param.x2, x3 = param.x3, x4 = param.x4;

    QVector<double> tValues = {0}, xValues = {x1}, yValues = {x2};
    tValues.reserve(param.k);
    xValues.reserve(param.k);
    yValues.reserve(param.k);

    for (uint i = 1; i < param.k; ++i) 
    {   
        // Коэффициенты k1
        double k1_x1 = param.h * x3;
        double k1_x2 = param.h * x4;
        double k1_x3 = param.h * dx3dt(t, x1, x2);
        double k1_x4 = param.h * dx4dt(t, x1, x2);
        
        // Коэффициенты k2
        double k2_x1 = param.h * (x3 + k1_x3 / 2);
        double k2_x2 = param.h * (x4 + k1_x4 / 2);
        double k2_x3 = param.h * dx3dt(t + param.h / 2, x1 + k1_x1 / 2, x2 + k1_x2 / 2);
        double k2_x4 = param.h * dx4dt(t + param.h / 2, x1 + k1_x1 / 2, x2 + k1_x2 / 2);
        
        // Коэффициенты k3
        double k3_x1 = param.h * (x3 + k2_x3 / 2);
        double k3_x2 = param.h * (x4 + k2_x4 / 2);
        double k3_x3 = param.h * dx3dt(t + param.h / 2, x1 + k2_x1 / 2, x2 + k2_x2 / 2);
        double k3_x4 = param.h * dx4dt(t + param.h / 2, x1 + k2_x1 / 2, x2 + k2_x2 / 2);
        
        // Коэффициенты k4
        double k4_x1 = param.h * (x3 + k3_x3);
        double k4_x2 = param.h * (x4 + k3_x4);
        double k4_x3 = param.h * dx3dt(t + param.h, x1 + k3_x1, x2 + k3_x2);
        double k4_x4 = param.h * dx4dt(t + param.h, x1 + k3_x1, x2 + k3_x2);
        
        // Обновление значений
        x1 += (k1_x1 + 2*k2_x1 + 2*k3_x1 + k4_x1) / 6;
        x2 += (k1_x2 + 2*k2_x2 + 2*k3_x2 + k4_x2) / 6;
        x3 += (k1_x3 + 2*k2_x3 + 2*k3_x3 + k4_x3) / 6;
        x4 += (k1_x4 + 2*k2_x4 + 2*k3_x4 + k4_x4) / 6;
        
        // Сохранение результатов
        t += param.h;
        tValues.append(t);
        xValues.append(x1);
        yValues.append(x2);
    }

    animatePlots(tValues, xValues, yValues);
}

void Osc2ndDegFree::initUI()
{
    initXCurve(&xtCurve, ui->plotTX);
    initXCurve(&xtCurve2, ui->plotTXY);
    initYCurve(&ytCurve, ui->plotTY);
    initYCurve(&ytCurve2, ui->plotTXY);

    yxCurve = new QCPCurve(ui->plotXY->xAxis, ui->plotXY->yAxis);
    yxCurve->setPen(QPen(Qt::blue));

    ui->plotTX->xAxis->setLabel("t");
    ui->plotTX->yAxis->setLabel("x");
    ui->plotTY->xAxis->setLabel("t");
    ui->plotTY->yAxis->setLabel("y");
    ui->plotTXY->xAxis->setLabel("t");
    ui->plotXY->xAxis->setLabel("x");
    ui->plotXY->yAxis->setLabel("y");

    connect(ui->pbCalc, &QPushButton::clicked, this, &Osc2ndDegFree::redrawPlots);
}

void Osc2ndDegFree::initXCurve(QCPCurve **xCurve, QCustomPlot *plot)
{
    // Создаем объект кривой
    *xCurve = new QCPCurve(plot->xAxis, plot->yAxis);
    (*xCurve)->setPen(QPen(Qt::red));
}

void Osc2ndDegFree::initYCurve(QCPCurve **yCurve, QCustomPlot *plot)
{
    // Создаем объект кривой
    *yCurve = new QCPCurve(plot->xAxis, plot->yAxis);
    (*yCurve)->setPen(QPen(Qt::darkGreen));
}

void Osc2ndDegFree::animatePlots(const QVector<double> &tValues, const QVector<double> &xValues, const QVector<double> &yValues)
{
    double x_min = *std::min_element(xValues.begin(), xValues.end());
    double x_max = *std::max_element(xValues.begin(), xValues.end());
    double y_min = *std::min_element(yValues.begin(), yValues.end());
    double y_max = *std::max_element(yValues.begin(), yValues.end());
    double t_max = *std::max_element(tValues.begin(), tValues.end());

    ui->plotTX->xAxis->setRange(0, t_max);
    ui->plotTX->yAxis->setRange(x_min, x_max);
    
    ui->plotTY->xAxis->setRange(0, t_max);
    ui->plotTY->yAxis->setRange(y_min, y_max);
    
    ui->plotTXY->xAxis->setRange(0, t_max);
    ui->plotTXY->yAxis->setRange(y_min < x_min ? y_min : x_min, y_max > x_max ? y_max : x_max);

    ui->plotXY->xAxis->setRange(x_min, x_max);
    ui->plotXY->yAxis->setRange(y_min, y_max);
    for (uint i = 0; i < tValues.size(); ++i)
    {
        xtCurve->addData(tValues[i], xValues[i]);
        xtCurve2->addData(tValues[i], xValues[i]);
        ytCurve->addData(tValues[i], yValues[i]);
        ytCurve2->addData(tValues[i], yValues[i]);
        yxCurve->addData(xValues[i], yValues[i]);

        ui->plotTX->replot();
        ui->plotTY->replot();
        ui->plotTXY->replot();
        ui->plotXY->replot();
    }
}

void Osc2ndDegFree::setParametersFromUI()
{
    param.x1 = ui->sbX10->value();
    param.x2 = ui->sbX20->value();
    param.x3 = ui->sbX30->value();
    param.x4 = ui->sbX40->value();
    
    param.A1 = ui->sbA1->value();
    param.A2 = ui->sbA2->value();
    param.p1 = ui->dsbP1->value();
    param.p2 = ui->dsbP2->value();

    param.alpha1 = ui->dsbAlpha1->value();
    param.alpha2 = ui->dsbAlpha2->value();
    param.v1 = ui->dsbV1->value();
    param.v2 = ui->dsbV2->value();

    param.k = ui->sbTime->value();
    param.h = ui->dsbAccuracy->value();
}

void Osc2ndDegFree::clearPlots()
{
    xtCurve->setData({}, {});
    xtCurve2->setData({}, {});
    ytCurve->setData({}, {});
    ytCurve2->setData({}, {});
    yxCurve->setData({}, {});
}

double Osc2ndDegFree::dx3dt(double t, double x1, double x2) const
{
    double res = param.A1 * cos(param.p1 * t);
    res += param.alpha1 * param.A2 * cos(param.p2 * t);
    res -= param.v1 * param.v1 * x1;
    res -= param.alpha1 * param.v2 * param.v2 * x2;
    return res / (1 - param.alpha1 * param.alpha2);
}

double Osc2ndDegFree::dx4dt(double t, double x1, double x2) const
{
    double res = dx3dt(t, x1, x2);
    res += param.v1 * param.v1 * x1 - param.A1 * cos(param.p1 * t);
    return res / param.alpha1;
}