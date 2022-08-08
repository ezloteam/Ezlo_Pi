#ifndef DIALOG_CONFIG_ONEWIRE_H
#define DIALOG_CONFIG_ONEWIRE_H

#include <QDialog>

namespace Ui {
class Dialog_config_onewire;
}

class Dialog_config_onewire : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_config_onewire(QWidget *parent = nullptr);
    ~Dialog_config_onewire();

private:
    Ui::Dialog_config_onewire *ui;
};

#endif // DIALOG_CONFIG_ONEWIRE_H
