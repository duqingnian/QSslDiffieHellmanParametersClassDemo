#include "Dialog.h"
#include "./ui_Dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    server = new Server();
    server->startServer(12345);
}

Dialog::~Dialog()
{
    delete ui;
}
