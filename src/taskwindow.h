#pragma once

#include <QMainWindow>

/// Абстракстный класс для всех виджетов с заданием
class TaskWindow : public QMainWindow
{
    Q_OBJECT

public:
    /// Конструктор
    explicit TaskWindow(QWidget *parent = nullptr) : QMainWindow(parent, Qt::Window)
    {
        setAttribute(Qt::WA_DeleteOnClose);
    }
    /// Деструктор
    virtual ~TaskWindow() = default;

protected:
    /// Инициализация параметров пользовательского интерфейса
    virtual void initUI() = 0;

    /// Перерисовать график
    void redrawPlot(QCustomPlot *plot)
    {
        plot->rescaleAxes();
        plot->replot();
    }
};