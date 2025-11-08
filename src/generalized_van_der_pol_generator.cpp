// ============================================================================
// Обобщённый генератор Ван дер Поля
// Уравнение: x'' - m*(1 - eps*x²)*x' + l*x³ + x = b*cos(p*t)
// ============================================================================

#include "generalized_van_der_pol_generator.h"
#include "qcustomplot.h"
#include "ui_generalized_van_der_pol_generator.h"
#include <cmath>
#include <QMessageBox>

using namespace gvdp;

namespace
{
    using Vector = QVector<double>;
}

GeneralizedVanDerPolGeneratorWindow::GeneralizedVanDerPolGeneratorWindow(QWidget *parent) 
    : TaskWindow(parent), ui(new Ui::GeneralizedVanDerPolGeneratorWindow), phasePortraitCurve(nullptr)
{
    ui->setupUi(this);
    initUI();
}

GeneralizedVanDerPolGeneratorWindow::~GeneralizedVanDerPolGeneratorWindow()
{
    delete ui;
}

void GeneralizedVanDerPolGeneratorWindow::redrawPlots()
{
    Parameters param = getParametersFromUI();
    Vector tValues, xValues, yValues;  // Для эволюционного графика
    Vector xValuesPhase, yValuesPhase;  // Для фазового портрета (все точки)

    // Проверка ограничений
    if (param.h <= 0) {
        QMessageBox::warning(this, "Ошибка", "Точность построения должна быть > 0");
        return;
    }
    if (param.timeStart > param.timeEnd) {
        QMessageBox::warning(this, "Ошибка", "Начало диапазона должно быть меньше или равно концу");
        return;
    }

    // Вычисляем количество точек на основе диапазона
    uint numPoints = static_cast<uint>((param.timeEnd - param.timeStart) / param.h) + 1;

    double t = 0.0;
    double x = param.x0;
    double y = param.y0;

    // ========================================================================
    // Если timeStart > 0, сначала эволюционируем до timeStart
    // (но записываем все точки для показа переходного процесса в фазовом портрете)
    // ========================================================================
    double tStart = static_cast<double>(param.timeStart);
    
    // Эволюционируем от t=0 до timeStart, записывая все точки для фазового портрета
    while (t < tStart)
    {
        // Записываем точку для фазового портрета (все точки с начала для показа переходного процесса)
        xValuesPhase.append(x);
        yValuesPhase.append(y);
        
        // Проверка на численные ошибки
        if (!std::isfinite(x) || !std::isfinite(y)) {
            break;
        }

        double k1x = param.h * y;
        double k1y = param.h * (param.m * (1.0 - param.eps * x*x) * y 
                                - param.l * x*x*x - x + param.b * std::cos(param.p * t));

        double k2x = param.h * (y + 0.5 * k1y);
        double x2 = x + 0.5*k1x;
        double k2y = param.h * (param.m * (1.0 - param.eps * x2*x2) * (y + 0.5*k1y)
                      - param.l * x2*x2*x2 - x2 
                      + param.b * std::cos(param.p * (t + 0.5*param.h)));

        double k3x = param.h * (y + 0.5 * k2y);
        double x3 = x + 0.5*k2x;
        double k3y = param.h * (param.m * (1.0 - param.eps * x3*x3) * (y + 0.5*k2y)
                      - param.l * x3*x3*x3 - x3 
                      + param.b * std::cos(param.p * (t + 0.5*param.h)));

        double k4x = param.h * (y + k3y);
        double x4 = x + k3x;
        double k4y = param.h * (param.m * (1.0 - param.eps * x4*x4) * (y + k3y)
                      - param.l * x4*x4*x4 - x4 
                      + param.b * std::cos(param.p * (t + param.h)));

        x = x + (k1x + 2.0*k2x + 2.0*k3x + k4x) / 6.0;
        y = y + (k1y + 2.0*k2y + 2.0*k3y + k4y) / 6.0;
        t += param.h;
    }

    // Теперь начинаем запись для эволюционного графика (начиная с timeStart)
    t = tStart;
    tValues.append(t);

    for (uint i = 1; i <= numPoints; ++i)
    {
        // Проверка на численные ошибки
        if (!std::isfinite(x) || !std::isfinite(y)) {
            break;
        }

        // Система уравнений:
        // dx/dt = y
        // dy/dt = m*(1 - eps*x²)*y - l*x³ - x + b*cos(p*t)

        double k1x = param.h * y;
        double k1y = param.h * (param.m * (1.0 - param.eps * x*x) * y 
                                - param.l * x*x*x - x + param.b * std::cos(param.p * t));

        double k2x = param.h * (y + 0.5 * k1y);
        double x2 = x + 0.5*k1x;
        double k2y = param.h * (param.m * (1.0 - param.eps * x2*x2) * (y + 0.5*k1y)
                      - param.l * x2*x2*x2 - x2 
                      + param.b * std::cos(param.p * (t + 0.5*param.h)));

        double k3x = param.h * (y + 0.5 * k2y);
        double x3 = x + 0.5*k2x;
        double k3y = param.h * (param.m * (1.0 - param.eps * x3*x3) * (y + 0.5*k2y)
                      - param.l * x3*x3*x3 - x3 
                      + param.b * std::cos(param.p * (t + 0.5*param.h)));

        double k4x = param.h * (y + k3y);
        double x4 = x + k3x;
        double k4y = param.h * (param.m * (1.0 - param.eps * x4*x4) * (y + k3y)
                      - param.l * x4*x4*x4 - x4 
                      + param.b * std::cos(param.p * (t + param.h)));

        x = x + (k1x + 2.0*k2x + 2.0*k3x + k4x) / 6.0;
        y = y + (k1y + 2.0*k2y + 2.0*k3y + k4y) / 6.0;
        t += param.h;

        // Записываем точку для обоих графиков
        tValues.append(t);
        xValues.append(x);
        yValues.append(y);
        xValuesPhase.append(x);
        yValuesPhase.append(y);
    }

    // Обновление эволюционного графика
    ui->oscillatorPlot->graph(0)->setData(tValues, xValues);
    ui->oscillatorPlot->rescaleAxes();
    ui->oscillatorPlot->replot();

    // Обновление фазового портрета (используем все точки для показа переходного процесса)
    if (phasePortraitCurve) {
        phasePortraitCurve->setData(xValuesPhase, yValuesPhase);
        ui->phasePortretPlot->rescaleAxes();
        ui->phasePortretPlot->replot();
    }
}

void GeneralizedVanDerPolGeneratorWindow::initUI()
{
    initOscillatorPlot();
    initPhasePortretPlot();

    // Установка значений по умолчанию (соответствуют изображению)
    ui->dsbX0->setValue(0.01);
    ui->dsbY0->setValue(0.01);
    ui->dsbM->setValue(1.5);
    ui->dsbEps->setValue(1.0);
    ui->dsbL->setValue(0.0);
    ui->dsbB->setValue(2.0);
    ui->dsbP->setValue(5.0);
    ui->dsbH->setValue(0.1);

    // Подключение кнопки вычисления
    connect(ui->pbCalculate, &QPushButton::clicked, this, &GeneralizedVanDerPolGeneratorWindow::redrawPlots);
}

void GeneralizedVanDerPolGeneratorWindow::initOscillatorPlot()
{
    ui->oscillatorPlot->xAxis->setLabel("t, c");
    ui->oscillatorPlot->yAxis->setLabel("x(t)");
    ui->oscillatorPlot->plotLayout()->insertRow(0);
    ui->oscillatorPlot->plotLayout()->addElement(0, 0, 
        new QCPTextElement(ui->oscillatorPlot, 
                          "Эволюционный график"));

    ui->oscillatorPlot->addGraph();
    ui->oscillatorPlot->graph(0)->setPen(QPen(Qt::blue, 1.5));
    ui->oscillatorPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->oscillatorPlot->graph(0)->setName("Эволюционный график");

    ui->oscillatorPlot->legend->setVisible(true);
    ui->oscillatorPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));
    ui->oscillatorPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);

    ui->oscillatorPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    connect(ui->oscillatorPlot, &QCustomPlot::mouseWheel, this, &GeneralizedVanDerPolGeneratorWindow::onOscillatorMouseWheel);
}

void GeneralizedVanDerPolGeneratorWindow::initPhasePortretPlot()
{
    ui->phasePortretPlot->xAxis->setLabel("x");
    ui->phasePortretPlot->yAxis->setLabel("y(dx/dt)");
    ui->phasePortretPlot->plotLayout()->insertRow(0);
    ui->phasePortretPlot->plotLayout()->addElement(0, 0, 
        new QCPTextElement(ui->phasePortretPlot, "Фазовый портрет"));

    // ВАЖНО: Используем QCPCurve для красивого отображения фазового портрета
    phasePortraitCurve = new QCPCurve(ui->phasePortretPlot->xAxis, ui->phasePortretPlot->yAxis);
    phasePortraitCurve->setPen(QPen(Qt::blue, 1.5));
    phasePortraitCurve->setName("Фазовый портрет");

    ui->phasePortretPlot->legend->setVisible(true);
    ui->phasePortretPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));
    ui->phasePortretPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);

    ui->phasePortretPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    connect(ui->phasePortretPlot, &QCustomPlot::mouseWheel, this, &GeneralizedVanDerPolGeneratorWindow::onPhasePortretMouseWheel);
}

void GeneralizedVanDerPolGeneratorWindow::onOscillatorMouseWheel(QWheelEvent *event)
{
    onMouseWheel(event, ui->oscillatorPlot);
}

void GeneralizedVanDerPolGeneratorWindow::onPhasePortretMouseWheel(QWheelEvent *event)
{
    onMouseWheel(event, ui->phasePortretPlot);
}

void GeneralizedVanDerPolGeneratorWindow::onMouseWheel(QWheelEvent *event, QCustomPlot *plot)
{
    if (!plot) return;

    QCPAxisRect *axisRect = plot->axisRect();
    double factor = (event->angleDelta().y() > 0) ? 0.85 : (1.0 / 0.85);

    if (axisRect->rangeDrag().testFlag(Qt::Horizontal)) {
        plot->xAxis->scaleRange(factor, plot->xAxis->pixelToCoord(event->position().x()));
    } else {
        plot->xAxis->scaleRange(factor);
    }

    if (axisRect->rangeDrag().testFlag(Qt::Vertical)) {
        plot->yAxis->scaleRange(factor, plot->yAxis->pixelToCoord(event->position().y()));
    } else {
        plot->yAxis->scaleRange(factor);
    }

    plot->replot();
    event->accept();
}

Parameters GeneralizedVanDerPolGeneratorWindow::getParametersFromUI() const
{
    Parameters result;
    result.x0 = ui->dsbX0->value();
    result.y0 = ui->dsbY0->value();
    result.m = ui->dsbM->value();
    result.eps = ui->dsbEps->value();
    result.l = ui->dsbL->value();
    result.b = ui->dsbB->value();
    result.p = ui->dsbP->value();
    result.h = ui->dsbH->value();
    result.timeStart = static_cast<uint>(ui->sbTimeStart->value());
    result.timeEnd = static_cast<uint>(ui->sbTimeEnd->value());
    return result;
}

void GeneralizedVanDerPolGeneratorWindow::redrawPlot(QCustomPlot *plot, 
                                            const Vector &xAxisPoints, 
                                            const Vector &yAxisPoints)
{
    plot->rescaleAxes();
    plot->replot();
}

void GeneralizedVanDerPolGeneratorWindow::updateTimeRangeDisplay()
{
    // Метод больше не нужен, так как диапазон редактируется напрямую
}

