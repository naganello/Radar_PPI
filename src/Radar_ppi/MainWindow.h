#pragma once

#include "MulticastConfigDialog.h"

#include <AsterixQt/AsterixService.h>

#include <QMainWindow>

class QLabel;
class PpiWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void configureMulticast();
    void startReception();
    void stopReception();
    void saveSocketConfigXml();
    void loadSocketConfigXml();

private:
    void createMenus();
    void updateStatus();
    bool writeSocketConfigXml(const QString& fileName) const;
    bool readSocketConfigXml(const QString& fileName);

    PpiWidget* ppi_ = nullptr;
    QLabel* status_ = nullptr;
    asterixqt::AsterixService* cat48Service_ = nullptr;
    asterixqt::AsterixService* cat240Service_ = nullptr;

    PpiMulticastConfig config_;
    asterixqt::DecoderStats cat48Stats_;
    asterixqt::DecoderStats cat240Stats_;
};
