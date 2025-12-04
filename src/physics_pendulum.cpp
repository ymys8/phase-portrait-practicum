#include "physics_pendulum.h"
#include "qcustomplot.h"
#include "ui_physics_pendulum.h"

#include <iostream>
#include <cmath>

using namespace pp;

PhysicsPendulumWindow::PhysicsPendulumWindow(QWidget *parent) : TaskWindow(parent), ui(new Ui::PhysicsPendulumWindow)
{
    ui->setupUi(this);
    initUI();
}

PhysicsPendulumWindow::~PhysicsPendulumWindow()
{
    delete ui;
}

void PhysicsPendulumWindow::redrawPlots()
{
    Parameters param = getParametrsFromUI();
    QVector<double> tValues, phiValues, psiValues;

    double t_end = param.t;
    int k = 1000;
    double h = t_end / k;

    // Метод Рунге-Кутты 4-го порядка
    // Начальные условия
    double t = 0;
    double x = param.phi0; // начальный угол
    double y = param.psi0; // начальная угловая скорость

    auto sgn = [](double value) -> double {
        if (value > 0) return 1.0;
        if (value < 0) return -1.0;
        return 0.0;
    };

    for (int i = 0; i <= k; ++i) {
        tValues.append(t);
        phiValues.append(x);
        psiValues.append(y);

        // Коэффициенты Рунге-Кутты для системы:
        // x' = y
        // y' = -2δ x sgn(x - x₀) - ω₀² sin(x) + A cos(ρ t)

        // k1
        double k1x = h * y;
        double k1y = h * (-2 * param.delta * y
                          - param.omega0 * sin(x)
                          + param.A * cos(param.rho * t));

        // k2
        double x2 = x + 0.5 * k1x;
        double y2 = y + 0.5 * k1y;
        double t2 = t + 0.5 * h;

        double k2x = h * y2;
        double k2y = h * (-2 * param.delta * y2
                          - param.omega0 * sin(x2)
                          + param.A * cos(param.rho * t2));

        // k3
        double x3 = x + 0.5 * k2x;
        double y3 = y + 0.5 * k2y;

        double k3x = h * y3;
        double k3y = h * (-2 * param.delta * y3
                          - param.omega0 * sin(x3)
                          + param.A * cos(param.rho * t2));  // t2 то же самое

        // k4
        double x4 = x + k3x;
        double y4 = y + k3y;
        double t4 = t + h;

        double k4x = h * y4;
        double k4y = h * (-2 * param.delta * y4
                          - param.omega0 * sin(x4)
                          + param.A * cos(param.rho * t4));

        x += (k1x + 2*k2x + 2*k3x + k4x) / 6.0;
        y += (k1y + 2*k2y + 2*k3y + k4y) / 6.0;
        t += h;
    }

    ui->evolutionXPlot->graph(0)->setData(tValues, phiValues);
    redrawPlot(ui->evolutionXPlot);

    ui->evolutionYPlot->graph(0)->setData(tValues, psiValues);
    redrawPlot(ui->evolutionYPlot);

    ui->evolutionXYPlot->graph(0)->setData(tValues, phiValues);
    ui->evolutionXYPlot->graph(1)->setData(tValues, psiValues);
    redrawPlot(ui->evolutionXYPlot);

    double maxX = 0.0;
    for (double phi : phiValues) {
        maxX = qMax(maxX, qAbs(phi));
    }

    double maxY = 0.0;
    for (double psi : psiValues) {
        maxY = qMax(maxY, qAbs(psi));
    }

    const double margin = 1.1;
    maxX *= margin;
    maxY *= margin;

    const double minRange = 0.1;
    maxX = qMax(maxX, minRange);
    maxY = qMax(maxY, minRange);

    // Устанавливаем симметричные диапазоны
    ui->phasePortretPlot->xAxis->setRange(-maxX, maxX);
    ui->phasePortretPlot->yAxis->setRange(-maxY, maxY);

    phasePortraitCurve->setData(phiValues, psiValues);
    ui->phasePortretPlot->replot();
    //redrawPlot(ui->phasePortretPlot);
}

void PhysicsPendulumWindow::initUI()
{
    initEvolutionXPlot();
    initEvolutionYPlot();
    initEvolutionXYPlot();
    initPhasePortretPlot();

    connect(ui->pbCalc, &QPushButton::clicked, this, &PhysicsPendulumWindow::redrawPlots);
}

void pp::PhysicsPendulumWindow::initEvolutionXPlot()
{
    ui->evolutionXPlot->xAxis->setLabel("t");
    ui->evolutionXPlot->yAxis->setLabel("x");

    // Добавление графика
    ui->evolutionXPlot->addGraph();
    ui->evolutionXPlot->graph(0)->setPen(QPen(Qt::red));
}

void pp::PhysicsPendulumWindow::initEvolutionYPlot()
{
    ui->evolutionYPlot->xAxis->setLabel("t");
    ui->evolutionYPlot->yAxis->setLabel("y");

    // Добавление графика
    ui->evolutionYPlot->addGraph();
    ui->evolutionYPlot->graph(0)->setPen(QPen(Qt::green));
}

void pp::PhysicsPendulumWindow::initEvolutionXYPlot()
{
    ui->evolutionXYPlot->xAxis->setLabel("t");
    ui->evolutionXYPlot->yAxis->setLabel("x, y");

    // Добавление графика
    ui->evolutionXYPlot->addGraph();
    ui->evolutionXYPlot->graph(0)->setPen(QPen(Qt::red));
    ui->evolutionXYPlot->addGraph();
    ui->evolutionXYPlot->graph(1)->setPen(QPen(Qt::green));
}

void pp::PhysicsPendulumWindow::initPhasePortretPlot()
{
    // Настройка осей и заголовка
    ui->phasePortretPlot->xAxis->setLabel("x");
    ui->phasePortretPlot->yAxis->setLabel("y");

    // Создаем объект кривой
    phasePortraitCurve = new QCPCurve(ui->phasePortretPlot->xAxis, ui->phasePortretPlot->yAxis);
    phasePortraitCurve->setPen(QPen(Qt::blue));
}

pp::Parameters pp::PhysicsPendulumWindow::getParametrsFromUI() const
{
    Parameters result;
    result.omega0 = ui->dsbOmega0->value();
    std::cout << "omega0 " << result.omega0 << std::endl;
    result.delta = ui->dsbDelta->value();
    std::cout << "delta " << result.delta << std::endl;
    result.A = ui->dsbA->value();
    std::cout << "A " << result.A << std::endl;
    result.rho = ui->dsbRho->value();
    std::cout << "rho " << result.rho << std::endl;
    result.t = ui->dsbT->value();
    std::cout << "t " << result.t << std::endl;
    result.phi0 = ui->dsbPhi0->value();
    result.psi0 = ui->dsbPsi0->value();

    return result;
}

void pp::PhysicsPendulumWindow::redrawPlot(QCustomPlot *plot)
{
    plot->rescaleAxes();
    plot->replot();
}
