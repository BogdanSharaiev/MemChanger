#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include "main.cpp"
#include <QStringListModel>
#include <QRegularExpression>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringListModel>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QComboBox* MainWindow::getComboBox() {
    return ui->comboBox;
}

void MainWindow::updateList(const QStringList& list) {
    QStringListModel* model = new QStringListModel(this);
    model->setStringList(list);
    ui->listView->setModel(model);
}



// Поиск по значению (сигнатуре)
void MainWindow::on_pushButton_clicked() {
    DWORD pid = getPid(ui->comboBox);
    int value = ui->lineEdit->text().toInt();



    scanMemory(pid, value);

    QStringList list;
    for (const auto& addr : addrs) {
        list << QString::number((qulonglong)addr, 16);
    }

    updateList(list);
}

// Фильтрация найденных адресов
void MainWindow::on_pushButton_2_clicked() {
    DWORD pid = getPid(ui->comboBox);
    int newVal = ui->lineEdit_2->text().toInt();

    filterAddresses(pid, newVal);

    QStringList list;
    for (const auto& addr : result) {
        list << QString::number((qulonglong)addr, 16);
    }

    updateList(list);
}

void MainWindow::on_pushButton_3_clicked() {
    DWORD pid = getPid(ui->comboBox);
    int newVal = ui->lineEdit_3->text().toInt();
    QModelIndex index = ui->listView->currentIndex();
    QString text = index.data().toString();
    uintptr_t addr = text.toULongLong(nullptr, 16);
    writeToAddress(pid, newVal, addr);

    qDebug() << "Values written.";
}





