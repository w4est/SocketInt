#include "clientform.h"
#include "ui_clientform.h"

ClientForm::ClientForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClientForm)
{
    ui->setupUi(this);
}

ClientForm::~ClientForm()
{
    delete ui;
}
