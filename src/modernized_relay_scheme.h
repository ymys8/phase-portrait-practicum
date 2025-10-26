#pragma once

#include "taskwindow.h"

namespace Ui
{
    class ModernizedRelayScheme;
}

class QCPCurve;

namespace mrs
{

struct Parameters
{
    double x0; ///< Начальный x
    double y0; ///< Начальный y
    double a1; ///< Коэффициент A1
    double a2; ///< Коэффициент A2
    double a3; ///< Коэффициент A3
    double a4; ///< Коэффициент A4
    double h;  ///< Временной интервал между точками
    uint k;    ///< Количество точек
};

/// Класс окна для модернизированной схемы Рэлея
class ModernizedRelaySchemeWindow : public TaskWindow
{
public:
    /// Конструктор
    explicit ModernizedRelaySchemeWindow(QWidget *parent = nullptr);
    /// Деструктор
    ~ModernizedRelaySchemeWindow();

private slots:
    /// Переримовать графики
    void redrawPlots();

private:
    /// Инициализация параметров пользовательского интерфейса
    virtual void initUI() override;
    /// Инициализровать настройки графика системы
    void initSchemePlot();
    /// Инициализировать настройки графика фазового портрета
    void initPhasePortraitPlot();

    /// Получить параметры из gui
    Parameters getParametersFromUI() const;

    /// Функция для вычисления производных системы (приведение уравнения к системе ОДУ)
    std::vector<double> derivative(const std::vector<double> &state, double A1, double A2, double A3, double A4);
    /// Метод Рунге-Кутты 4-го порядка для одного шага
    void rungeKuttaStep(std::vector<double> &state, double h, double A1, double A2, double A3, double A4);

    Ui::ModernizedRelayScheme *ui; ///< Форма
    QCPCurve *phasePortraitCurve;  ///< Кривая прозодящая через точки фазового портрета
};

};