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
    string PortToSend = this->ui->HostPort->toPlainText().toStdString();
    if (atoi (PortToSend.c_str()) <= 0){ //Make sure that the port number is usable (>-1)
        //This isn't going to work.
        this->ui->ConsoleText->setText("Please use a valid port number\n");
    }
    else{
        SocketClient ClientThing(atoi(PortToSend.c_str())); //Send the port
    }
}

void MainWindow::on_HostButton_clicked()
{
    SocketHost HostThing(this); //Also give host access to ui elements to read
}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}
