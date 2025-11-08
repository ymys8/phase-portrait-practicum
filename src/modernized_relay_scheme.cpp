#include "modernized_relay_scheme.h"
#include "ui_modernized_relay_scheme.h"

#include <iostream>

using namespace mrs;

ModernizedRelaySchemeWindow::ModernizedRelaySchemeWindow(QWidget *parent) : TaskWindow(parent), ui(new Ui::ModernizedRelayScheme)
{
    ui->setupUi(this);
    initUI();
}

ModernizedRelaySchemeWindow::~ModernizedRelaySchemeWindow()
{
    delete ui;
}

void ModernizedRelaySchemeWindow::redrawPlots()
{
    Parameters param = getParametersFromUI();

    std::vector<double> state = {param.x0, param.y0};
    double t = 0.0;
    QVector<double> tValues, xValues, yValues;

    // Численное интегрирование методом Рунге-Кутты
    for (int step = 0; step < param.k; ++step) 
    {
        tValues.append(t);
        xValues.append(state[0]);
        yValues.append(state[1]);

        rungeKuttaStep(state, param.h, param.a1, param.a2, param.a3, param.a4);
        t += param.h;
    }
    
    ui->systemPlot->graph(0)->setData(tValues, xValues);
    ui->systemPlot->rescaleAxes();  
    ui->systemPlot->replot();
    
    phasePortraitCurve->setData(xValues, yValues);
    ui->phasePortraitPlot->rescaleAxes();  
    ui->phasePortraitPlot->replot();
}

void ModernizedRelaySchemeWindow::initUI()
{
    initSchemePlot();
    initPhasePortraitPlot();

    connect(ui->pbCalc, &QPushButton::clicked, this, &ModernizedRelaySchemeWindow::redrawPlots);
}

void ModernizedRelaySchemeWindow::initSchemePlot()
{
    // Настройка осей и заголовка
    ui->systemPlot->xAxis->setLabel("t");
    ui->systemPlot->yAxis->setLabel("x");
    ui->systemPlot->xAxis->setRange(0, 10);
    ui->systemPlot->yAxis->setRange(-2, 2);
    ui->systemPlot->plotLayout()->insertRow(0);
    ui->systemPlot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->systemPlot, "Колебательная система: x(t)"));

    // Добавление графика
    ui->systemPlot->addGraph();
    ui->systemPlot->graph(0)->setPen(QPen(Qt::red));
}

void ModernizedRelaySchemeWindow::initPhasePortraitPlot()
{
    // Настройка осей и заголовка
    ui->phasePortraitPlot->xAxis->setLabel("x");
    ui->phasePortraitPlot->yAxis->setLabel("y");
    ui->phasePortraitPlot->plotLayout()->insertRow(0);
    ui->phasePortraitPlot->plotLayout()->addElement(0, 0, new QCPTextElement(ui->phasePortraitPlot, "Фазовый портрет: y(x)"));

    // Создаем объект кривой
    phasePortraitCurve = new QCPCurve(ui->phasePortraitPlot->xAxis, ui->phasePortraitPlot->yAxis);
    phasePortraitCurve->setPen(QPen(Qt::blue));
}

Parameters ModernizedRelaySchemeWindow::getParametersFromUI() const
{
    Parameters result;
    result.x0 = ui->dsbX0->value();
    result.y0 = ui->dsbY0->value();
    result.a1 = ui->dsbA1->value();
    result.a2 = ui->dsbA2->value();
    result.a3 = ui->dsbA3->value();
    result.a4 = ui->dsbA4->value();
    result.h = ui->dsbH->value();
    result.k = ui->sbK->value();

    return result;
}

std::vector<double> ModernizedRelaySchemeWindow::derivative(const std::vector<double> &state, double A1, 
                                                            double A2, double A3, double A4)
{
    if (state.size() < 2)
    {
        std::cerr << "Недостаточно данных для подсчета производных" << std::endl;
        return {};
    }

    double x = state[0];
    double dx_dt = state[1];
    double d2x_dt2 = -A1 * dx_dt * (1 + A2 * dx_dt + A3 * dx_dt * dx_dt) - x * (1 + A4 * x * x);
    return {dx_dt, d2x_dt2};
}

void ModernizedRelaySchemeWindow::rungeKuttaStep(std::vector<double> &state, double h, double A1, 
                                                double A2, double A3, double A4)
{
    auto k1 = derivative(state, A1, A2, A3, A4);
    std::vector<double> state_temp = {state[0] + 0.5 * h * k1[0], state[1] + 0.5 * h * k1[1]};
    auto k2 = derivative(state_temp, A1, A2, A3, A4);
    state_temp = {state[0] + 0.5 * h * k2[0], state[1] + 0.5 * h * k2[1]};
    auto k3 = derivative(state_temp, A1, A2, A3, A4);
    state_temp = {state[0] + h * k3[0], state[1] + h * k3[1]};
    auto k4 = derivative(state_temp, A1, A2, A3, A4);
    
    state[0] += (h / 6.0) * (k1[0] + 2*k2[0] + 2*k3[0] + k4[0]);
    state[1] += (h / 6.0) * (k1[1] + 2*k2[1] + 2*k3[1] + k4[1]);
}
