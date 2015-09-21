#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "socketclient.h"
#include "sockethost.h"


using namespace std;



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ClientButton_clicked()
{
    SocketClient ClientThing;
}

void MainWindow::on_HostButton_clicked()
{
    SocketHost HostThing(this); //Also give host access to ui elements to read
}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}
