// ============================================================================
// ИСПРАВЛЕННАЯ ВЕРСИЯ: van_der_pol_generator.cpp
// Правый график (фазовый портрет): используется QCPCurve + стабилизация
// Левый график (временной): используется QCPGraph + стабилизация
// ============================================================================
#include "van_der_pol_generator.h"
#include "qcustomplot.h"
#include "ui_van_der_pol_generator.h"
#include <cmath>
#include <limits>
#include <QMessageBox>

using namespace vdp;

namespace
{
    using Vector = QVector<double>;
};

VanDerPolGeneratorWindow::VanDerPolGeneratorWindow(QWidget *parent) 
    : TaskWindow(parent), ui(new Ui::VanDerPolGeneratorWindow), phasePortraitCurve(nullptr)
{
    ui->setupUi(this);
    // Инициализация кэшированных параметров (чтобы первое вычисление всегда выполнялось)
    cachedParams.x0 = std::numeric_limits<double>::quiet_NaN();
    initUI();
}

VanDerPolGeneratorWindow::~VanDerPolGeneratorWindow()
{
    delete ui;
}

void VanDerPolGeneratorWindow::redrawPlots()
{
    Parameters param = getParametersFromUI();
    
    // Проверяем, изменились ли параметры
    bool paramsChanged = (param.x0 != cachedParams.x0 || param.y0 != cachedParams.y0 ||
                         param.a1 != cachedParams.a1 || param.a2 != cachedParams.a2 ||
                         param.a3 != cachedParams.a3 || param.h != cachedParams.h ||
                         param.k != cachedParams.k);
    
    Vector tValues, xValues;  // Для эволюционного графика
    Vector xValuesPhase, yValuesPhase;  // Для фазового портрета (все точки с начала)
    
    // Используем кэш, если параметры не изменились
    if (!paramsChanged && !cachedTValues.isEmpty()) {
        tValues = cachedTValues;
        xValues = cachedXValues;
        xValuesPhase = cachedXValuesPhase;
        yValuesPhase = cachedYValuesPhase;
    } else {
        // Вычисляем новые данные
        calculateData(param, tValues, xValues, xValuesPhase, yValuesPhase);
        
        // Сохраняем в кэш
        cachedTValues = tValues;
        cachedXValues = xValues;
        cachedXValuesPhase = xValuesPhase;
        cachedYValuesPhase = yValuesPhase;
        cachedParams = param;
    }

    // Обновление графиков
    Vector emptyYValues;  // Не используется для эволюционного графика
    updatePlots(tValues, xValues, emptyYValues, xValuesPhase, yValuesPhase);
}

void VanDerPolGeneratorWindow::calculateData(const Parameters& param, 
                                             Vector& tValues, Vector& xValues,
                                             Vector& xValuesPhase, Vector& yValuesPhase)
{
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
    // Вычисление с записью с самого начала для обоих графиков
    // Эволюционный график записывается с t=0 (как на изображении)
    // ============================================================================
    
    // Записываем начальную точку для обоих графиков
    tValues.append(0.0);
    xValues.append(x);  // Для эволюционного графика: x(t)
    xValuesPhase.append(x);  // Для фазового портрета
    yValuesPhase.append(y);
    
    for (int i = 1; i <= param.k; ++i)
    {
        // Проверка на численные ошибки
        if (!std::isfinite(x) || !std::isfinite(y)) {
            break;
        }

        // Записываем все точки для фазового портрета (после вычисления следующей точки)
        // Но сначала вычисляем следующую точку
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
        
        // Записываем точку для обоих графиков
        tValues.append(t);
        xValues.append(x);  // Для эволюционного графика: x(t)
        xValuesPhase.append(x);  // Для фазового портрета
        yValuesPhase.append(y);
    }

}

void VanDerPolGeneratorWindow::updatePlots(const Vector& tValues, const Vector& xValues, const Vector& /*yValues*/,
                                          const Vector& xValuesPhase, const Vector& yValuesPhase)
{
    // ============================================================================
    // ОБНОВЛЕНИЕ ГРАФИКОВ
    // ============================================================================

    // ЛЕВЫЙ график: x(t) - используем QCPGraph (обычная линия)
    ui->oscillatorPlot->graph(0)->setData(tValues, xValues);
    // Не используем rescaleAxes, чтобы сохранить фиксированные диапазоны как на изображении
    ui->oscillatorPlot->replot(QCustomPlot::rpQueuedReplot); // Используем очередь для оптимизации

    // ПРАВЫЙ график: фазовый портрет y(x) - используем QCPCurve (все точки с начала)
    if (phasePortraitCurve) {
        phasePortraitCurve->setData(xValuesPhase, yValuesPhase);
        ui->phasePortretPlot->rescaleAxes();
        ui->phasePortretPlot->replot(QCustomPlot::rpQueuedReplot); // Используем очередь для оптимизации
    }
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
    // Настройка ЛЕВОГО графика x(t) - соответствует изображению
    ui->oscillatorPlot->xAxis->setLabel("T");
    ui->oscillatorPlot->yAxis->setLabel("X");
    ui->oscillatorPlot->xAxis->setRange(0, 100);
    ui->oscillatorPlot->yAxis->setRange(-2.1, 2.1);
    ui->oscillatorPlot->plotLayout()->insertRow(0);
    ui->oscillatorPlot->plotLayout()->addElement(0, 0, 
        new QCPTextElement(ui->oscillatorPlot, "Модифицированный генератор Ван-дер-Поля: x(t)"));

    // Добавление графика QCPGraph
    ui->oscillatorPlot->addGraph();
    ui->oscillatorPlot->graph(0)->setPen(QPen(Qt::red, 1.5));
    ui->oscillatorPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    
    // Включение адаптивной выборки для оптимизации производительности
    ui->oscillatorPlot->graph(0)->setAdaptiveSampling(true);
    
    // Оптимизация отрисовки
    ui->oscillatorPlot->setNotAntialiasedElements(QCP::aeAll);
    ui->oscillatorPlot->setAntialiasedElements(QCP::aeNone);
    ui->oscillatorPlot->setNoAntialiasingOnDrag(true);

    ui->oscillatorPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    
    // Отключаем кастомные обработчики мыши, используем встроенный zoom QCustomPlot
    // connect(ui->oscillatorPlot, &QCustomPlot::mouseWheel, this, &VanDerPolGeneratorWindow::onOscillatorMouseWheel);
}

void VanDerPolGeneratorWindow::initPhasePortretPlot()
{
    // Настройка осей и заголовка
    ui->phasePortretPlot->xAxis->setLabel("x");
    ui->phasePortretPlot->yAxis->setLabel("y");
    ui->phasePortretPlot->xAxis->setRange(-2, 2);
    ui->phasePortretPlot->yAxis->setRange(-4, 4);
    ui->phasePortretPlot->plotLayout()->insertRow(0);
    ui->phasePortretPlot->plotLayout()->addElement(0, 0, 
        new QCPTextElement(ui->phasePortretPlot, "Фазовый портрет: y(x)"));

    // ВАЖНО: Используем QCPCurve вместо QCPGraph для красивого отображения
    phasePortraitCurve = new QCPCurve(ui->phasePortretPlot->xAxis, ui->phasePortretPlot->yAxis);
    phasePortraitCurve->setPen(QPen(Qt::blue, 1.5));
    
    // Примечание: QCPCurve не поддерживает setAdaptiveSampling, но оптимизируем отрисовку
    // Оптимизация отрисовки
    ui->phasePortretPlot->setNotAntialiasedElements(QCP::aeAll);
    ui->phasePortretPlot->setAntialiasedElements(QCP::aeNone);
    ui->phasePortretPlot->setNoAntialiasingOnDrag(true);

    // Включение взаимодействия (QCustomPlot автоматически обрабатывает zoom)
    ui->phasePortretPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void VanDerPolGeneratorWindow::onOscillatorMouseWheel(QWheelEvent *event)
{
    // Используем встроенный механизм зума QCustomPlot для оптимизации
    // QCustomPlot автоматически обрабатывает zoom через iRangeZoom
    // Просто обновляем график без пересчета данных
    ui->oscillatorPlot->replot(QCustomPlot::rpQueuedReplot);
    event->accept();
}

void VanDerPolGeneratorWindow::onPhasePortretMouseWheel(QWheelEvent *event)
{
    // Используем встроенный механизм зума QCustomPlot для оптимизации
    // QCustomPlot автоматически обрабатывает zoom через iRangeZoom
    // Просто обновляем график без пересчета данных
    ui->phasePortretPlot->replot(QCustomPlot::rpQueuedReplot);
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