#ifndef DIALOG_CONFIG_I2C_H
#define DIALOG_CONFIG_I2C_H

#include <QDialog>

namespace Ui {
class Dialog_config_i2c;
}

class Dialog_config_i2c : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_config_i2c(QWidget *parent = nullptr);
    ~Dialog_config_i2c();

private:
    Ui::Dialog_config_i2c *ui;
};

#endif // DIALOG_CONFIG_I2C_H
