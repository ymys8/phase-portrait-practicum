#include "mainwindow.h"
#include "linear_oscillator_system.h"
#include "modernized_relay_scheme.h"
#include "ui_mainwindow.h"

#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QMessageBox>

const auto TASK_TYPE_ROLE = Qt::UserRole;
const QString DOCS_PATH = "../documents";
Q_DECLARE_METATYPE(ETaskWindowType)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initTaskList();
    initTextList();

    connect(ui->listTask, &QListView::doubleClicked, this, &MainWindow::showTaskWindow);
    connect(ui->listDocs, &QListView::doubleClicked, [](const QModelIndex &index) {
        QString fileName = index.data().toString();
        QDesktopServices::openUrl(QUrl::fromLocalFile(DOCS_PATH + "\\" + fileName));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    for(auto task : actualWindows) {
        if (!task.isNull())
        {
            task->close();
        }
    }
    
    event->accept();
}

void MainWindow::showTaskWindow(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    auto type = index.data(TASK_TYPE_ROLE).value<ETaskWindowType>();

    if (actualWindows.contains(type))
    {
        QPointer<TaskWindow> window = actualWindows[type];
        if (!window.isNull())
        {
            window->raise();
            window->activateWindow();
            return;
        }
        else
        {
            actualWindows.remove(type);
        }
    }

    QPointer<TaskWindow> window = createTaskWindow(type);
    if (window == nullptr)
    {
        QMessageBox::critical(this, "Ошибка при создании окна с заданием", 
                                tr("Невозможно создать задание: %1").arg(index.data(Qt::DisplayRole).toString()));
        return;
    }
    actualWindows.insert(type, window);
    window->show();
}

void MainWindow::initTaskList()
{
    QStandardItemModel *model = new QStandardItemModel(this);
    ui->listTask->setModel(model);

    auto appendTaskRow = [model](const QString &name, ETaskWindowType type)
    {
        QStandardItem* item = new QStandardItem(name);
        model->appendRow(item);
        model->setData(model->index(model->rowCount() - 1, 0), QVariant::fromValue(type), TASK_TYPE_ROLE);
    };

    appendTaskRow("Линейная колебательная система", ETaskWindowType::LOS);
    appendTaskRow("Модернизированная схема Рэлея", ETaskWindowType::MRS);
}

void MainWindow::initTextList()
{
    QDir dir(DOCS_PATH);
    QStringList files = dir.entryList({"*.doc", "*.docx"}, QDir::Files, QDir::Name);

    auto model = new QStringListModel(this);
    model->setStringList(files);
    ui->listDocs->setModel(model);
}

QPointer<TaskWindow> MainWindow::createTaskWindow(ETaskWindowType windowType)
{
    QPointer<TaskWindow> resPtr;

    switch (windowType)
    {
        case ETaskWindowType::LOS:
            resPtr = new los::LinearOscillatorSystemWindow;
            break;
        case ETaskWindowType::MRS:
            resPtr = new mrs::ModernizedRelaySchemeWindow;
            break;
        default:
            return nullptr;
    }

    return resPtr;
}