#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>

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

void MainWindow::on_loadFileButton_clicked()
{
    m_fileName = QFileDialog::getOpenFileName(this, tr("Please select an audio file"));
    qDebug() << "fileName: " << m_fileName;
}

double MainWindow::sign(double x)
{
    if (x < 0) return -1;
    return 1;
}

void MainWindow::encode()
{
    const char *fname1 = "input.wav";
    const char *fname2 = "out.wav";
    cBitStream in (fname1, mRead);
    cBitStream out (fname2, mWrite);
    unsigned long int codeWord;
    double x = 0;
    double F = 0;

    short word = 0;

    for (int i = 0; i < 44; i++)
    {
        in.ReadBitField(&codeWord, 8);
        out.WriteBitField(codeWord, 8);
    }

    while(!in.eof())
    {
        in.ReadBitField(&codeWord,16);
        word = codeWord;
        x = word / 32767.0;
        if (abs(x) <= 1/A)
        {
            F = sign(x)*(A*abs(x)/(1 + log(A)));
        }
        else
        {
            F = sign(x)*(1 + log(abs(A*x))/(1 + log(A)));
        }
        F = F*pow(2.0, codeLen);
        codeWord = F;
        out.WriteBitField(codeWord, codeLen);
    }
}

void MainWindow::decode()
{
    const char *fname1 = "out.wav";
    const char *fname2 = "reb.wav";
    unsigned long int CodeWord;

    cBitStream in(fname1, mRead);
    cBitStream reb(fname2, mWrite);
    for (int i = 0; i < 44; i++)
    {	in.ReadBitField(&CodeWord, 8);
        reb.WriteBitField(CodeWord,8);
    }
    while (!in.eof())
    {
        in.ReadBitField(&CodeWord, codeLen);

        short cw;
        float y;
        if (CodeWord > pow(2.0, codeLen-1))
            cw = - (pow(2.0, codeLen)-CodeWord);
        else cw = CodeWord;
        y = abs(cw/pow(2.0, (codeLen-1)));
        float x;
        if (y <= 1/(1+log(A)))
            x = (sign(cw)*y*(1+log(A)))/A;
        else
            x = (sign(cw)*exp(y*(1+log(A))-1))/A;
        x*=32767;
        CodeWord = x;

        reb.WriteBitField(CodeWord, 16);
    }
}

void MainWindow::on_encodeButton_clicked()
{
    encode();
}

void MainWindow::on_decodeButton_clicked()
{
    decode();
}
