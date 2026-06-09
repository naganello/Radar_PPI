#pragma once

#include <QDialog>
#include <QString>

class QLineEdit;
class QSpinBox;

struct MulticastEndpointConfig
{
    QString group;
    quint16 port = 0;
    QString local;
};

struct PpiMulticastConfig
{
    MulticastEndpointConfig cat48;
    MulticastEndpointConfig cat240;
};

class MulticastConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MulticastConfigDialog(const PpiMulticastConfig& config,
                                   QWidget* parent = nullptr);

    PpiMulticastConfig config() const;

private:
    QLineEdit* cat48Group_ = nullptr;
    QSpinBox* cat48Port_ = nullptr;
    QLineEdit* cat240Group_ = nullptr;
    QSpinBox* cat240Port_ = nullptr;
    QLineEdit* local_ = nullptr;
};
