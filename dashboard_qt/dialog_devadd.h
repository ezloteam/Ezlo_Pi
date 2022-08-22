#ifndef DIALOG_DEVADD_H
#define DIALOG_DEVADD_H

#include <QDialog>
#include <ezlopi.h>

namespace Ui {
class Dialog_devadd;
}

class Dialog_devadd : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_devadd(QWidget *parent = nullptr);
    ~Dialog_devadd();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Dialog_devadd *ui;

signals:
void ezpi_send_dev_type_selected(EZPI_UINT8);

};

#endif // DIALOG_DEVADD_H
