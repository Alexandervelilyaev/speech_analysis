#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mIsListening(false)
{
    ui->setupUi(this);

    microphone = new Microphone();
    microphone->initialize();

    connect(microphone, &Microphone::valueChanged, this, &MainWindow::onMicrophoneValueChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_toggleListeningButton_clicked()
{
    mIsListening = !mIsListening;
    ui->toggleListeningButton->setText(mIsListening ? "Stop" : "Start");
}

void MainWindow::onMicrophoneValueChanged(int value)
{
    ui->progressBar->setValue(value);
}
