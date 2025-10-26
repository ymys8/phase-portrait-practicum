#pragma once

#include "taskwindow.h"

namespace Ui
{
    class VanDerPolGeneratorWindow;
}

class QCustomPlot;
class QCPCurve;

namespace vdp
{

/// Параметры для генератора Ван-дер-Поля
struct Parameters
{
    double x0; ///< Начальный x
    double y0; ///< Начальный y
    double a1; ///< Коэффициент A1
    double a2; ///< Коэффициент A2 (A2 > 0)
    double a3; ///< Коэффициент A3 (0.1 <= A3 <= 10)
    double h;  ///< Временной интервал между точками
    uint k;    ///< Количество точек
};

/// Класс окна для генератора Ван-дер-Поля
class VanDerPolGeneratorWindow : public TaskWindow
{
    Q_OBJECT

public:
    /// Конструктор
    explicit VanDerPolGeneratorWindow(QWidget *parent = nullptr);
    /// Деструктор
    ~VanDerPolGeneratorWindow();

private slots:
    /// Перерисовать графики
    void redrawPlots();
    void onOscillatorMouseWheel(QWheelEvent *event);
    void onPhasePortretMouseWheel(QWheelEvent *event);
    void onMouseWheel(QWheelEvent *event, QCustomPlot *plot);

private:
    /// Инициализация параметров пользовательского интерфейса
    virtual void initUI() override;
    /// Инициализровать настройки графика колебательной системы
    void initOscillatorPlot();
    /// Инициализировать настройки графика фазового портрета
    void initPhasePortretPlot();

    /// Получить параметры из gui
    Parameters getParametersFromUI() const;
    
    /// Перерисовать график
    void redrawPlot(QCustomPlot *plot, const QVector<double> &xAxisPoints, const QVector<double> &yAxisPoints);

    Ui::VanDerPolGeneratorWindow *ui; ///< Форма
    QCPCurve *phasePortraitCurve;     ///< Кривая прозодящая через точки фазового портрета
};

}