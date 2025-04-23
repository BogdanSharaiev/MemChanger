#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QMainWindow>
#include <vector>
#include <windows.h>
#include <QApplication>
#include "tlhelp32.h"
#include <QComboBox>
#include <QString>
#include <vector>
#include <QMediaPlayer>
#include <QUrl>
#include <QFile>
#include <QMediaPlayer>
#include <QNetworkAccessManager>
#include <fstream>
#include <QDir>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QComboBox* getComboBox();
    void updateList(const QStringList& list);


private slots:
    // Обработчики событий
    void on_pushButton_clicked();  // Кнопка поиска
    void on_pushButton_2_clicked();  // Кнопка фильтрации
    void on_pushButton_3_clicked();  // Кнопка записи в память

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
