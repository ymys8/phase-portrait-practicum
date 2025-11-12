#pragma once

#include "taskwindow.h"

namespace Ui
{
    class Osc2ndDegFree;
}

namespace o2df
{

struct Parameters
{
    int x1;
    int x2;
    int x3;
    int x4;
    double v1;
    double v2;
    double alpha1;
    double alpha2;
    int A1;
    int A2;
    double p1;
    double p2;
    int k;
    double h;
};

/// Класс окна для колебаний 2 степени свободы
class Osc2ndDegFree : public TaskWindow
{
    Q_OBJECT

public:
    /// Конструктор
    explicit Osc2ndDegFree(QWidget *parent = nullptr);
    /// Деструктор
    ~Osc2ndDegFree();

private slots:
    /// Перерисовать графики
    void redrawPlots();

private:
    /// Инициализировать ui окна
    void initUI() override;
    /// Инициализировать график x(t)
    void initXCurve(QCPCurve **xCurve, QCustomPlot *plot);
    /// Инициализировать график y(t)
    void initYCurve(QCPCurve **yCurve, QCustomPlot *plot);

    /// Запустить отрисовку графиков
    void animatePlots(const QVector<double> &tValues, const QVector<double> &xValues, const QVector<double> &yValues);

    /// Получить параметры из gui
    void setParametersFromUI();
    /// Очистить графики
    void clearPlots();
    
    /// Получить x3 (x1')
    double dx3dt(double t, double x1, double x2) const;
    /// Получить x4 (x2')
    double dx4dt(double t, double x1, double x2) const;

    Ui::Osc2ndDegFree *ui;   ///< Форма
    QCPCurve *xtCurve;       ///< Кривая x(t)
    QCPCurve *ytCurve;       ///< Кривая y(t)
    QCPCurve *xtCurve2;      ///< Кривая x(t) для совместного графика
    QCPCurve *ytCurve2;      ///< Кривая y(t) для совместного графика
    QCPCurve *yxCurve;       ///< Кривая y(x)
    Parameters param;        ///< Параметры из формы
};

};