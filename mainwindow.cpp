#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mIsListening = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_toggleListeningButton_clicked()
{
    mIsListening = !mIsListening;
    ui->toggleListeningButton->setText(mIsListening ? "Stop" : "Start");
    //qDebug() << "click";

    ui->progressBar->setValue(50);
}
