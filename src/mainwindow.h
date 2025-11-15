#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QMainWindow>
#include <QPointer>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class TaskWindow;

/// Перечисление типов окон с заданием
enum class ETaskWindowType
{
    LOS = 1, ///< Линейная колебательная система
    PP = 2   ///< Физический маятник
};

/// Класс основного окна
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /// Конструктор
    MainWindow(QWidget *parent = nullptr);
    /// Деструктор
    ~MainWindow();

protected:
    /// Закрытие окон с заданиями при закрытии основного окна
    void closeEvent(QCloseEvent *event) override;

private slots:
    /// Показать окно задания
    void showTaskWindow(const QModelIndex &index);

private:
    /// Инициализировать список с заданиями (левый)
    void initTaskList();
    /// Инициализировать список с докуметнацией (правый)
    void initTextList();

    /// Создать окно задания конкретного типа
    QPointer<TaskWindow> createTaskWindow(ETaskWindowType windowType);

    Ui::MainWindow *ui;                                        ///< Форма
    QMap<ETaskWindowType, QPointer<TaskWindow>> actualWindows; ///< Активные открытые окна заданий
};
#endif // MAINWINDOW_H
