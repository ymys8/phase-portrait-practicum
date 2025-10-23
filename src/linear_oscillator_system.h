#pragma once

#include "taskwindow.h"

namespace Ui
{
    class LinearOscillatorSystemWindow;
}

class QCustomPlot;
class QCPCurve;

namespace los
{

/// Параметры необходимые для вычислений
struct Parameters
{
    double x0; ///< Начальный x
    double y0; ///< Начальный y
    double a1; ///< Коэффициент A1
    double a2; ///< Коэффициент A2
    double h;  ///< Временной интервал между точками
    uint k;    ///< Количество точек
};

/// Класс окна для линейной колебательной системы
class LinearOscillatorSystemWindow : public TaskWindow
{
    Q_OBJECT

public:
    /// Конструктор
    explicit LinearOscillatorSystemWindow(QWidget *parent = nullptr);
    /// Деструктор
    ~LinearOscillatorSystemWindow();

private slots:
    /// Перерисовать графики
    void redrawPlots();

private:
    /// Инициализация параметров пользовательского интерфейса
    virtual void initUI() override;
    /// Инициализровать настройки графика линейной колебательной системы
    void initLinearOscillatorPlot();
    /// Инициализировать настройки графика фазового портрета
    void initPhasePortretPlot();

    /// Получить параметры из gui
    Parameters getParametersFromUI() const;
    
    /// Перерисовать график
    void redrawPlot(QCustomPlot *plot, const QVector<double> &xAxisPoints, const QVector<double> &yAxisPoints);

    Ui::LinearOscillatorSystemWindow *ui; ///< Форма
    QCPCurve *phasePortraitCurve;         ///< Кривая прозодящая через точки фазового портрета
};

};