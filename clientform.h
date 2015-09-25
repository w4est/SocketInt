#ifndef CLIENTFORM_H
#define CLIENTFORM_H

#include <QDialog>

namespace Ui {
class ClientForm;
}

class ClientForm : public QDialog
{
    Q_OBJECT

public:
    explicit ClientForm(QWidget *parent = 0);
    ~ClientForm();

private:
    Ui::ClientForm *ui;
};

#endif // CLIENTFORM_H
