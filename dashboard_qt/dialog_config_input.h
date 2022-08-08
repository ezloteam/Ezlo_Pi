#ifndef DIALOG_CONFIG_INPUT_H
#define DIALOG_CONFIG_INPUT_H

#include <QDialog>

namespace Ui {
class Dialog_config_input;
}

class Dialog_config_input : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_config_input(QWidget *parent = nullptr);
    ~Dialog_config_input();

private:
    Ui::Dialog_config_input *ui;
};

#endif // DIALOG_CONFIG_INPUT_H
