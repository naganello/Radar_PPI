#include "MulticastConfigDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>

MulticastConfigDialog::MulticastConfigDialog(const PpiMulticastConfig& config,
                                             QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Configurazione multicast ASTERIX"));

    cat48Group_ = new QLineEdit(config.cat48.group, this);
    cat48Port_ = new QSpinBox(this);
    cat48Port_->setRange(1, 65535);
    cat48Port_->setValue(config.cat48.port);

    cat240Group_ = new QLineEdit(config.cat240.group, this);
    cat240Port_ = new QSpinBox(this);
    cat240Port_->setRange(1, 65535);
    cat240Port_->setValue(config.cat240.port);

    local_ = new QLineEdit(config.cat48.local, this);

    auto* cat48Box = new QGroupBox(tr("CAT048"), this);
    auto* cat48Layout = new QFormLayout(cat48Box);
    cat48Layout->addRow(tr("Gruppo"), cat48Group_);
    cat48Layout->addRow(tr("Porta"), cat48Port_);

    auto* cat240Box = new QGroupBox(tr("CAT240"), this);
    auto* cat240Layout = new QFormLayout(cat240Box);
    cat240Layout->addRow(tr("Gruppo"), cat240Group_);
    cat240Layout->addRow(tr("Porta"), cat240Port_);

    auto* localBox = new QGroupBox(tr("Interfaccia locale"), this);
    auto* localLayout = new QFormLayout(localBox);
    localLayout->addRow(tr("Bind address"), local_);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                         this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(cat48Box);
    layout->addWidget(cat240Box);
    layout->addWidget(localBox);
    layout->addWidget(buttons);
}

PpiMulticastConfig MulticastConfigDialog::config() const
{
    PpiMulticastConfig config;
    config.cat48 = {cat48Group_->text().trimmed(), quint16(cat48Port_->value()), local_->text().trimmed()};
    config.cat240 = {cat240Group_->text().trimmed(), quint16(cat240Port_->value()), local_->text().trimmed()};
    return config;
}
