#include "Dialog.h"
#include "./ui_Dialoglog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    client = new Client();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_btn_connect_clicked()
{
    client->connectToServer("localhost",12345);
}


void Dialog::on_btn_send_clicked()
{
    QString msg = ui->edit_message->text();
    client->sendMessage(msg);
}

