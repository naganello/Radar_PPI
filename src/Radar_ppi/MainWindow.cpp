#include "MainWindow.h"
#include "PpiWidget.h"

#include <QAction>
#include <QActionGroup>
#include <QDomDocument>
#include <QFile>
#include <QFileDialog>
#include <QHostAddress>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QTextStream>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    config_.cat48 = {"239.1.1.48", 30048, "0.0.0.0"};
    config_.cat240 = {"239.1.1.240", 30240, "0.0.0.0"};

    ppi_ = new PpiWidget(this);
    setCentralWidget(ppi_);

    status_ = new QLabel(this);
    statusBar()->addPermanentWidget(status_, 1);

    cat48Service_ = new asterixqt::AsterixService(this);
    cat240Service_ = new asterixqt::AsterixService(this);

    connect(cat48Service_, &asterixqt::AsterixService::cat048Decoded,
            ppi_, &PpiWidget::addTargets);
    connect(cat240Service_, &asterixqt::AsterixService::cat240Decoded,
            ppi_, &PpiWidget::addRadials);
    connect(cat48Service_, &asterixqt::AsterixService::statsUpdated,
            this,
            [this](const asterixqt::DecoderStats& stats) {
                cat48Stats_ = stats;
                updateStatus();
            });
    connect(cat240Service_, &asterixqt::AsterixService::statsUpdated,
            this,
            [this](const asterixqt::DecoderStats& stats) {
                cat240Stats_ = stats;
                updateStatus();
            });

    createMenus();
    startReception();
}

MainWindow::~MainWindow()
{
    stopReception();
}

void MainWindow::createMenus()
{
    auto* multicastMenu = menuBar()->addMenu("Multicast");
    auto* configureAction = multicastMenu->addAction("Configura CAT048/CAT240...");
    auto* loadXmlAction = multicastMenu->addAction("Carica socket XML...");
    auto* saveXmlAction = multicastMenu->addAction("Salva socket XML...");
    auto* startAction = multicastMenu->addAction("Avvia ricezione");
    auto* stopAction = multicastMenu->addAction("Ferma ricezione");

    connect(configureAction, &QAction::triggered, this, &MainWindow::configureMulticast);
    connect(loadXmlAction, &QAction::triggered, this, &MainWindow::loadSocketConfigXml);
    connect(saveXmlAction, &QAction::triggered, this, &MainWindow::saveSocketConfigXml);
    connect(startAction, &QAction::triggered, this, &MainWindow::startReception);
    connect(stopAction, &QAction::triggered, this, &MainWindow::stopReception);

    auto* ppiMenu = menuBar()->addMenu("PPI");

    auto* frameRateMenu = ppiMenu->addMenu("Frame rate");
    auto* fps30Action = frameRateMenu->addAction("30 FPS");
    auto* fps50Action = frameRateMenu->addAction("50 FPS");
    connect(fps30Action, &QAction::triggered, ppi_, [this]() { ppi_->setFrameRate(30); });
    connect(fps50Action, &QAction::triggered, ppi_, [this]() { ppi_->setFrameRate(50); });

    auto* renderMenu = ppiMenu->addMenu("Rendering video");
    auto* textureAction = renderMenu->addAction("Texture persistente");
    textureAction->setCheckable(true);
    textureAction->setChecked(true);
    connect(textureAction, &QAction::toggled, ppi_, &PpiWidget::setPersistentTextureEnabled);

    auto* doubleBufferAction = renderMenu->addAction("Doppio buffer");
    doubleBufferAction->setCheckable(true);
    connect(doubleBufferAction, &QAction::toggled, ppi_, &PpiWidget::setDoubleBufferEnabled);

    auto* tripleBufferAction = renderMenu->addAction("Triple buffering");
    tripleBufferAction->setCheckable(true);
    tripleBufferAction->setChecked(true);
    connect(tripleBufferAction, &QAction::toggled, ppi_, &PpiWidget::setTripleBufferEnabled);

    auto* decayAction = renderMenu->addAction("Decay sweep/video");
    decayAction->setCheckable(true);
    decayAction->setChecked(true);
    connect(decayAction, &QAction::toggled, ppi_, &PpiWidget::setSweepDecayEnabled);

    auto* rangeMenu = ppiMenu->addMenu("Scala range");
    auto* rangeGroup = new QActionGroup(this);

    for (int rangeNm : {1, 2, 5, 10, 25}) {
        auto* action = rangeMenu->addAction(QString("%1 NM").arg(rangeNm));
        action->setCheckable(true);
        action->setData(rangeNm);

        if (rangeNm == 25) {
            action->setChecked(true);
        }

        rangeGroup->addAction(action);

        connect(action, &QAction::triggered,
                this,
                [this, action]() {
                    ppi_->setRangeNm(action->data().toDouble());
                });
    }

    auto* clearAction = ppiMenu->addAction("Pulisci");
    connect(clearAction, &QAction::triggered, ppi_, &PpiWidget::clear);
}

void MainWindow::configureMulticast()
{
    MulticastConfigDialog dialog(config_, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    config_ = dialog.config();
    stopReception();
    startReception();
}

void MainWindow::startReception()
{
    const QHostAddress local(config_.cat48.local);
    const bool cat48Ok = cat48Service_->start(QHostAddress(config_.cat48.group),
                                              config_.cat48.port,
                                              local);
    const bool cat240Ok = cat240Service_->start(QHostAddress(config_.cat240.group),
                                                config_.cat240.port,
                                                local);

    if (!cat48Ok || !cat240Ok) {
        QMessageBox::warning(this, "Multicast", "Avvio ricezione parziale o fallito");
    }

    updateStatus();
}

void MainWindow::stopReception()
{
    if (cat48Service_) {
        cat48Service_->stop();
    }

    if (cat240Service_) {
        cat240Service_->stop();
    }
}

void MainWindow::saveSocketConfigXml()
{
    const QString fileName = QFileDialog::getSaveFileName(this,
                                                          tr("Salva configurazione socket"),
                                                          QStringLiteral("radar_socket_config.xml"),
                                                          tr("XML files (*.xml)"));

    if (!fileName.isEmpty() && !writeSocketConfigXml(fileName)) {
        QMessageBox::warning(this, tr("XML"), tr("Impossibile salvare il file XML."));
    }
}

void MainWindow::loadSocketConfigXml()
{
    const QString fileName = QFileDialog::getOpenFileName(this,
                                                          tr("Carica configurazione socket"),
                                                          QString(),
                                                          tr("XML files (*.xml)"));

    if (fileName.isEmpty()) {
        return;
    }

    if (!readSocketConfigXml(fileName)) {
        QMessageBox::warning(this, tr("XML"), tr("Impossibile leggere il file XML."));
        return;
    }

    stopReception();
    startReception();
}

bool MainWindow::writeSocketConfigXml(const QString& fileName) const
{
    QDomDocument document("RadarSocketConfig");
    QDomElement root = document.createElement("RadarSocketConfig");
    document.appendChild(root);

    auto appendEndpoint = [&](const QString& name, const MulticastEndpointConfig& endpoint) {
        QDomElement element = document.createElement(name);
        element.setAttribute("group", endpoint.group);
        element.setAttribute("port", endpoint.port);
        element.setAttribute("local", endpoint.local);
        root.appendChild(element);
    };

    appendEndpoint("CAT048", config_.cat48);
    appendEndpoint("CAT240", config_.cat240);

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return false;
    }

    QTextStream stream(&file);
    stream << document.toString(4);
    return true;
}

bool MainWindow::readSocketConfigXml(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QDomDocument document;
    if (!document.setContent(&file)) {
        return false;
    }

    const QDomElement root = document.documentElement();
    if (root.tagName() != "RadarSocketConfig") {
        return false;
    }

    auto readEndpoint = [&](const QString& name, MulticastEndpointConfig& endpoint) -> bool {
        const QDomElement element = root.firstChildElement(name);
        if (element.isNull()) {
            return false;
        }

        endpoint.group = element.attribute("group");
        endpoint.port = quint16(element.attribute("port").toUShort());
        endpoint.local = element.attribute("local", QStringLiteral("0.0.0.0"));
        return !endpoint.group.isEmpty() && endpoint.port > 0;
    };

    return readEndpoint("CAT048", config_.cat48)
        && readEndpoint("CAT240", config_.cat240);
}

void MainWindow::updateStatus()
{
    status_->setText(QString("CAT048 %1 target | CAT240 %2 radiali")
                         .arg(cat48Stats_.cat048TargetsDecoded)
                         .arg(cat240Stats_.cat240RadialsDecoded));
}
