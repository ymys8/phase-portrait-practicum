// ============================================================================
// ИСПРАВЛЕННАЯ ВЕРСИЯ: van_der_pol_generator.cpp
// Правый график (фазовый портрет): используется QCPCurve + стабилизация
// Левый график (временной): используется QCPGraph + стабилизация
// ============================================================================
#include "van_der_pol_generator.h"
#include "qcustomplot.h"
#include "ui_van_der_pol_generator.h"
#include <cmath>

using namespace vdp;

namespace
{
    using Vector = QVector<double>;
};

VanDerPolGeneratorWindow::VanDerPolGeneratorWindow(QWidget *parent) : TaskWindow(parent), ui(new Ui::VanDerPolGeneratorWindow)
{
    ui->setupUi(this);
    initUI();
}

VanDerPolGeneratorWindow::~VanDerPolGeneratorWindow()
{
    delete ui;
}

void VanDerPolGeneratorWindow::redrawPlots()
{
    Parameters param = getParametersFromUI();
    Vector tValues, xValues, yValues;

    // Проверка ограничений
    if (param.a2 < 0) {
        QMessageBox::warning(this, "Ошибка", "A2 должен быть >= 0");
        return;
    }
    if (param.a3 < 0.1 || param.a3 > 10) {
        QMessageBox::warning(this, "Ошибка", "A3 должен быть в диапазоне [0.1, 10]");
        return;
    }

    double t = 0.0;
    double x = param.x0;
    double y = param.y0;

    // ============================================================================
    // ШАГ 1: Стабилизация (пропускаем переходный процесс)
    // Это необходимо для обоих графиков!
    // ============================================================================
    int stabilizationSteps = 1000;
    for (int i = 0; i < stabilizationSteps; ++i)
    {
        double k1x = param.h * y;
        double k1y = param.h * (param.a1 * (1.0 - x*x) * y - x + param.a2 * std::cos(param.a3 * t));

        double k2x = param.h * (y + 0.5 * k1y);
        double k2y = param.h * (param.a1 * (1.0 - (x + 0.5*k1x)*(x + 0.5*k1x)) * (y + 0.5*k1y)
                      - (x + 0.5*k1x) + param.a2 * std::cos(param.a3 * (t + 0.5*param.h)));

        double k3x = param.h * (y + 0.5 * k2y);
        double k3y = param.h * (param.a1 * (1.0 - (x + 0.5*k2x)*(x + 0.5*k2x)) * (y + 0.5*k2y)
                      - (x + 0.5*k2x) + param.a2 * std::cos(param.a3 * (t + 0.5*param.h)));

        double k4x = param.h * (y + k3y);
        double k4y = param.h * (param.a1 * (1.0 - (x + k3x)*(x + k3x)) * (y + k3y)
                      - (x + k3x) + param.a2 * std::cos(param.a3 * (t + param.h)));

        x = x + (k1x + 2.0*k2x + 2.0*k3x + k4x) / 6.0;
        y = y + (k1y + 2.0*k2y + 2.0*k3y + k4y) / 6.0;
        t += param.h;
    }

    // ============================================================================
    // ШАГ 2: Основное вычисление (НЕ сбрасываем x, y, t!)
    // Продолжаем со стабилизированных значений
    // ============================================================================
    for (int i = 0; i <= param.k; ++i)
    {
        tValues.append(t);
        xValues.append(x);
        yValues.append(y);

        // Проверка на численные ошибки
        if (!std::isfinite(x) || !std::isfinite(y)) {
            break;
        }

        double k1x = param.h * y;
        double k1y = param.h * (param.a1 * (1.0 - x*x) * y - x + param.a2 * std::cos(param.a3 * t));

        double k2x = param.h * (y + 0.5 * k1y);
        double k2y = param.h * (param.a1 * (1.0 - (x + 0.5*k1x)*(x + 0.5*k1x)) * (y + 0.5*k1y)
                      - (x + 0.5*k1x) + param.a2 * std::cos(param.a3 * (t + 0.5*param.h)));

        double k3x = param.h * (y + 0.5 * k2y);
        double k3y = param.h * (param.a1 * (1.0 - (x + 0.5*k2x)*(x + 0.5*k2x)) * (y + 0.5*k2y)
                      - (x + 0.5*k2x) + param.a2 * std::cos(param.a3 * (t + 0.5*param.h)));

        double k4x = param.h * (y + k3y);
        double k4y = param.h * (param.a1 * (1.0 - (x + k3x)*(x + k3x)) * (y + k3y)
                      - (x + k3x) + param.a2 * std::cos(param.a3 * (t + param.h)));

        x = x + (k1x + 2.0*k2x + 2.0*k3x + k4x) / 6.0;
        y = y + (k1y + 2.0*k2y + 2.0*k3y + k4y) / 6.0;
        t += param.h;
    }

    // ============================================================================
    // ОБНОВЛЕНИЕ ГРАФИКОВ
    // ============================================================================

    // ЛЕВЫЙ график: x(t) - используем QCPGraph (обычная линия)
    ui->oscillatorPlot->graph(0)->setData(tValues, xValues);
    redrawPlot(ui->oscillatorPlot, tValues, xValues);

    // ПРАВЫЙ график: фазовый портрет y(x) - используем QCPCurve (гладкая кривая)
    if (phasePortraitCurve) {
        phasePortraitCurve->setData(xValues, yValues);
    }
    redrawPlot(ui->phasePortretPlot, xValues, yValues);
}


void VanDerPolGeneratorWindow::initUI()
{
    initOscillatorPlot();
    initPhasePortretPlot();

    // Установка значений по умолчанию
    ui->dsbX0->setValue(0.01);
    ui->dsbY0->setValue(0.01);
    ui->dsbA1->setValue(1.5);
    ui->dsbA2->setValue(5.0);
    ui->dsbA3->setValue(5.0);
    ui->dsbH->setValue(0.01);
    ui->sbK->setValue(10000);

    connect(ui->pbCalc, &QPushButton::clicked, this, &VanDerPolGeneratorWindow::redrawPlots);
}

void VanDerPolGeneratorWindow::initOscillatorPlot()
{
    // Настройка ЛЕВОГО графика x(t)
    ui->oscillatorPlot->xAxis->setLabel("t");
    ui->oscillatorPlot->yAxis->setLabel("x");
    ui->oscillatorPlot->plotLayout()->insertRow(0);
    ui->oscillatorPlot->plotLayout()->addElement(0, 0, 
        new QCPTextElement(ui->oscillatorPlot, "Модифицированный генератор Ван-дер-Поля: x(t)"));

    // Добавление графика QCPGraph
    ui->oscillatorPlot->addGraph();
    ui->oscillatorPlot->graph(0)->setPen(QPen(Qt::red, 1.5));
    ui->oscillatorPlot->graph(0)->setLineStyle(QCPGraph::lsLine);

    ui->oscillatorPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void VanDerPolGeneratorWindow::initPhasePortretPlot()
{
    // Настройка осей и заголовка
    ui->phasePortretPlot->xAxis->setLabel("x");
    ui->phasePortretPlot->yAxis->setLabel("y");
    ui->phasePortretPlot->xAxis->setRange(-4, 4);
    ui->phasePortretPlot->yAxis->setRange(-5, 5);
    ui->phasePortretPlot->plotLayout()->insertRow(0);
    ui->phasePortretPlot->plotLayout()->addElement(0, 0, 
        new QCPTextElement(ui->phasePortretPlot, "Фазовый портрет: y(x)"));

    // ВАЖНО: Используем QCPCurve вместо QCPGraph для красивого отображения
    phasePortraitCurve = new QCPCurve(ui->phasePortretPlot->xAxis, ui->phasePortretPlot->yAxis);
    phasePortraitCurve->setPen(QPen(Qt::blue, 1.5));

    // Включение взаимодействия
    ui->phasePortretPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // Подключение обработчика колеса мыши
    connect(ui->phasePortretPlot, &QCustomPlot::mouseWheel, this, &VanDerPolGeneratorWindow::onPhasePortretMouseWheel);
}

void VanDerPolGeneratorWindow::onOscillatorMouseWheel(QWheelEvent *event)
{
    // Масштабирование с помощью колеса мыши
    QCPAxisRect *axisRect = ui->oscillatorPlot->axisRect();
    double factor = 1.0;
    
    if (event->angleDelta().y() > 0) {
        factor = 0.85; // Приближение
    } else {
        factor = 1.0 / 0.85; // Отдаление
    }
    
    axisRect->rangeDrag().testFlag(Qt::Horizontal) ? 
        ui->oscillatorPlot->xAxis->scaleRange(factor, ui->oscillatorPlot->xAxis->pixelToCoord(event->position().x())) :
        ui->oscillatorPlot->xAxis->scaleRange(factor);
    
    axisRect->rangeDrag().testFlag(Qt::Vertical) ? 
        ui->oscillatorPlot->yAxis->scaleRange(factor, ui->oscillatorPlot->yAxis->pixelToCoord(event->position().y())) :
        ui->oscillatorPlot->yAxis->scaleRange(factor);
    
    ui->oscillatorPlot->replot();
    event->accept();
}

void VanDerPolGeneratorWindow::onPhasePortretMouseWheel(QWheelEvent *event)
{
    // Масштабирование с помощью колеса мыши
    QCPAxisRect *axisRect = ui->phasePortretPlot->axisRect();
    double factor = 1.0;
    
    if (event->angleDelta().y() > 0) {
        factor = 0.85; // Приближение
    } else {
        factor = 1.0 / 0.85; // Отдаление
    }
    
    axisRect->rangeDrag().testFlag(Qt::Horizontal) ? 
        ui->phasePortretPlot->xAxis->scaleRange(factor, ui->phasePortretPlot->xAxis->pixelToCoord(event->position().x())) :
        ui->phasePortretPlot->xAxis->scaleRange(factor);
    
    axisRect->rangeDrag().testFlag(Qt::Vertical) ? 
        ui->phasePortretPlot->yAxis->scaleRange(factor, ui->phasePortretPlot->yAxis->pixelToCoord(event->position().y())) :
        ui->phasePortretPlot->yAxis->scaleRange(factor);
    
    ui->phasePortretPlot->replot();
    event->accept();
}

// Альтернативный вариант - универсальный обработчик для обоих графиков
void VanDerPolGeneratorWindow::onMouseWheel(QWheelEvent *event, QCustomPlot *plot)
{
    if (!plot) return;
    
    QCPAxisRect *axisRect = plot->axisRect();
    double factor = 1.0;
    
    if (event->angleDelta().y() > 0) {
        factor = 0.85; // Приближение
    } else {
        factor = 1.0 / 0.85; // Отдаление
    }
    
    // Масштабирование относительно позиции курсора
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

Parameters VanDerPolGeneratorWindow::getParametersFromUI() const
{
    Parameters result;
    result.x0 = ui->dsbX0->value();
    result.y0 = ui->dsbY0->value();
    result.a1 = ui->dsbA1->value();
    result.a2 = ui->dsbA2->value();
    result.a3 = ui->dsbA3->value();
    result.h = ui->dsbH->value();
    result.k = ui->sbK->value();

    return result;
}

void VanDerPolGeneratorWindow::redrawPlot(QCustomPlot *plot, const Vector &xAxisPoints, const Vector &yAxisPoints)
{
    plot->rescaleAxes();
    plot->replot();
}