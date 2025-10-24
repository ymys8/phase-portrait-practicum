#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initTextList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initTextList()
{
    QDir dir(QDir::homePath() + "/phase-portrait-practicum/documents");

    QStringList files = dir.entryList({"*.doc", "*.docx"}, QDir::Files, QDir::Name);

    std::unique_ptr<QStringListModel> model = std::make_unique<QStringListModel>();
    model->setStringList(files);
    ui->lw_documents->setModel(model.release());

    QObject::connect(ui->lw_documents, &QListView::doubleClicked, [dir](const QModelIndex &index) {
        QString fileName = index.data().toString();
        QDesktopServices::openUrl(QUrl::fromLocalFile(dir.absoluteFilePath(fileName)));
    });
}
