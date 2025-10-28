#include "linear_oscillator_system.h"
#include "qcustomplot.h"
#include "ui_linear_oscillator_system.h"

using namespace los;

namespace
{
    using Vector = QVector<double>;
};

LinearOscillatorSystemWindow::LinearOscillatorSystemWindow(QWidget *parent) : TaskWindow(parent), ui(new Ui::LinearOscillatorSystemWindow)
{
    ui->setupUi(this);
    initUI();
}

LinearOscillatorSystemWindow::~LinearOscillatorSystemWindow()
{
    delete ui;
}

void LinearOscillatorSystemWindow::redrawPlots()
{
    Parameters param = getParametersFromUI();
    Vector tValues, xValues, yValues;

    // Метод Рунге-Кутты 4-го порядка
    double t = 0;
    double x = param.x0;
    double y = param.y0;

    for (int i = 0; i <= param.k; ++i)
    {
        tValues.append(t);
        xValues.append(x);
        yValues.append(y);

        double k1x = param.h * y;
        double k1y = param.h * (-param.a2 * x - param.a1 * y);

        double k2x = param.h * (y + 0.5 * k1y);
        double k2y = param.h * (-param.a2 * (x + 0.5 * k1x) - param.a1 * (y + 0.5 * k1y));

        double k3x = param.h * (y + 0.5 * k2y);
        double k3y = param.h * (-param.a2 * (x + 0.5 * k2x) - param.a1 * (y + 0.5 * k2y));

        double k4x = param.h * (y + k3y);
        double k4y = param.h * (-param.a2 * (x + k3x) - param.a1 * (y + k3y));

        double x_new = x + (k1x + 2 * k2x + 2 * k3x + k4x) / 6;
        double y_new = y + (k1y + 2 * k2y + 2 * k3y + k4y) / 6;

        x = x_new;
        y = y_new;
        t += param.h;
    }

    ui->linearOscillatorPlot->graph(0)->setData(tValues, xValues);
    redrawPlot(ui->linearOscillatorPlot);
    phasePortraitCurve->setData(xValues, yValues);
    redrawPlot(ui->phasePortretPlot);
}

void LinearOscillatorSystemWindow::initUI()
{
    initLinearOscillatorPlot();
    initPhasePortretPlot();

    connect(ui->pbCalc, &QPushButton::clicked, this, &LinearOscillatorSystemWindow::redrawPlots);
}

void LinearOscillatorSystemWindow::initLinearOscillatorPlot()
{
    // Настройка осей и заголовка
    ui->linearOscillatorPlot->xAxis->setLabel("t");
    ui->linearOscillatorPlot->yAxis->setLabel("x");
    ui->linearOscillatorPlot->xAxis->setRange(0, 10);
    ui->linearOscillatorPlot->yAxis->setRange(-2, 2);
    ui->linearOscillatorPlot->plotLayout()->insertRow(0);
    ui->linearOscillatorPlot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->linearOscillatorPlot, "Колебательная система: x(t)"));

    // Добавление графика
    ui->linearOscillatorPlot->addGraph();
    ui->linearOscillatorPlot->graph(0)->setPen(QPen(Qt::red));
}

void LinearOscillatorSystemWindow::initPhasePortretPlot()
{
    // Настройка осей и заголовка
    ui->phasePortretPlot->xAxis->setLabel("x");
    ui->phasePortretPlot->yAxis->setLabel("y");
    ui->phasePortretPlot->plotLayout()->insertRow(0);
    ui->phasePortretPlot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->phasePortretPlot, "Фазовый портрет: y(x)"));

    // Создаем объект кривой
    phasePortraitCurve = new QCPCurve(ui->phasePortretPlot->xAxis, ui->phasePortretPlot->yAxis);
    phasePortraitCurve->setPen(QPen(Qt::blue));
}

Parameters LinearOscillatorSystemWindow::getParametersFromUI() const
{
    Parameters result;
    result.x0 = ui->dsbX0->value();
    result.y0 = ui->dsbY0->value();
    result.a1 = ui->dsbA1->value();
    result.a2 = ui->dsbA2->value();
    result.h = ui->dsbH->value();
    result.k = ui->sbK->value();

    return result;
}