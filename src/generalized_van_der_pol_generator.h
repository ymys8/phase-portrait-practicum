#pragma once

#include "taskwindow.h"

namespace Ui
{
    class GeneralizedVanDerPolGeneratorWindow;
}

class QCustomPlot;
class QCPCurve;

namespace gvdp
{
    /// Параметры для обобщённого генератора Ван дер Поля
    /// Уравнение: x'' - m*(1 - eps*x²)*x' + l*x³ + x = b*cos(p*t)
    struct Parameters
    {
        double x0;   ///< Начальный x
        double y0;   ///< Начальный y (dx/dt)
        double m;    ///< Коэффициент m (нелинейное демпфирование)
        double eps;  ///< Коэффициент eps (нелинейность)
        double l;    ///< Коэффициент l (нелинейная жёсткость)
        double b;    ///< Коэффициент b (амплитуда внешней силы)
        double p;    ///< Коэффициент p (частота внешней силы)
        double h;    ///< Временной интервал между точками (точность построения)
        uint timeStart; ///< Начало диапазона по оси абсцисс
        uint timeEnd;   ///< Конец диапазона по оси абсцисс
    };

    /// Класс окна для обобщённого генератора Ван дер Поля
    class GeneralizedVanDerPolGeneratorWindow : public TaskWindow
    {
        Q_OBJECT

    public:
        /// Конструктор
        explicit GeneralizedVanDerPolGeneratorWindow(QWidget *parent = nullptr);

        /// Деструктор
        ~GeneralizedVanDerPolGeneratorWindow();

    private slots:
        /// Перерисовать графики
        void redrawPlots();

        void onOscillatorMouseWheel(QWheelEvent *event);
        void onPhasePortretMouseWheel(QWheelEvent *event);
        void onMouseWheel(QWheelEvent *event, QCustomPlot *plot);

    private:
        /// Инициализация параметров пользовательского интерфейса
        virtual void initUI() override;

        /// Инициализировать настройки графика колебательной системы
        void initOscillatorPlot();

        /// Инициализировать настройки графика фазового портрета
        void initPhasePortretPlot();

        /// Получить параметры из GUI
        Parameters getParametersFromUI() const;

        /// Перерисовать график
        void redrawPlot(QCustomPlot *plot, const QVector<double> &xAxisPoints, 
                       const QVector<double> &yAxisPoints);

        /// Обновить отображение диапазона времени
        void updateTimeRangeDisplay();

        Ui::GeneralizedVanDerPolGeneratorWindow *ui;  ///< Форма
        QCPCurve *phasePortraitCurve;        ///< Кривая фазового портрета
    };
}

