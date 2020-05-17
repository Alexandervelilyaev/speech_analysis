#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "cBitStream.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    const double A = 87.56;
    const double codeLen = 12;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_loadFileButton_clicked();

    void on_encodeButton_clicked();

    void on_decodeButton_clicked();

private:
    Ui::MainWindow *ui;
    QString m_fileName;

    double sign(double x);
    void encode();
    void decode();
    short decoder(short CodeWord);
};
#endif // MAINWINDOW_H
