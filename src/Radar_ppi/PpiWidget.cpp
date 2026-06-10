#include "PpiWidget.h"

#include <QPainter>
#include <QVector2D>
#include <QtMath>

#include <cmath>

namespace {

constexpr double LightSpeedMps = 299792458.0;
constexpr double MetersPerNm = 1852.0;
constexpr int MaxVideoCellsPerRadial = 512;

} // namespace

PpiWidget::PpiWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , vbo_(QOpenGLBuffer::VertexBuffer)
{
    setMouseTracking(true);

    connect(&repaintTimer_, &QTimer::timeout,
            this,
            [this]() {
                update();
            });

    setFrameRate(30);
    decayTimer_.start();
}

PpiWidget::~PpiWidget()
{
    makeCurrent();

    if (textureId_ != 0) {
        glDeleteTextures(1, &textureId_);
    }

    vbo_.destroy();
    vao_.destroy();
    doneCurrent();
}

int PpiWidget::frameRate() const
{
    return frameRate_;
}

void PpiWidget::setFrameRate(int fps)
{
    frameRate_ = qBound(1, fps, 120);
    repaintTimer_.start(1000 / frameRate_);
}

void PpiWidget::setPersistentTextureEnabled(bool enabled)
{
    persistentTexture_ = enabled;
    clearImages();
}

void PpiWidget::setDoubleBufferEnabled(bool enabled)
{
    doubleBuffer_ = enabled;
    if (enabled) {
        tripleBuffer_ = false;
    }
    clearImages();
}

void PpiWidget::setTripleBufferEnabled(bool enabled)
{
    tripleBuffer_ = enabled;
    if (enabled) {
        doubleBuffer_ = false;
    }
    clearImages();
}

void PpiWidget::setSweepDecayEnabled(bool enabled)
{
    decayEnabled_ = enabled;
}

void PpiWidget::setSweepDecayMs(int decayMs)
{
    decayMs_ = qBound(20, decayMs, 2000);
}

void PpiWidget::setRangeNm(double rangeNm)
{
    rangeNm_ = qBound(1.0, rangeNm, 25.0);
    clearImages();
}

void PpiWidget::zoomIn()
{
    setRangeNm(rangeNm_ / 1.25);
}

void PpiWidget::zoomOut()
{
    setRangeNm(rangeNm_ * 1.25);
}

void PpiWidget::resetView()
{
    rangeNm_ = 25.0;
    zoom_ = 1.0;
    clearImages();
}

void PpiWidget::clear()
{
    QMutexLocker locker(&mutex_);
    targets_.clear();
    radials_.clear();
    selectedTargetIndex_ = -1;
    clearImages();
}

void PpiWidget::addTargets(const QVector<asterixqt::Cat048Target>& targets)
{
    QMutexLocker locker(&mutex_);

    for (const auto& target : targets) {
        if (target.hasTrackNumber) {
            bool updated = false;

            for (auto& existingTarget : targets_) {
                if (existingTarget.hasTrackNumber && existingTarget.trackNumber == target.trackNumber) {
                    existingTarget = target;
                    updated = true;
                    break;
                }
            }

            if (!updated) {
                targets_.push_back(target);
            }
        } else {
            targets_.push_back(target);
        }

        if (target.hasPolar) {
            sweepAzimuthDeg_ = target.azimuthDeg;
        }
    }

    if (targets_.size() > 4096) {
        targets_.erase(targets_.begin(), targets_.begin() + (targets_.size() - 4096));
    }

    if (selectedIsTrack_) {
        selectedTargetIndex_ = -1;
        for (int i = 0; i < targets_.size(); ++i) {
            if (targets_[i].hasTrackNumber && targets_[i].trackNumber == selectedTrackNumber_) {
                selectedTargetIndex_ = i;
                break;
            }
        }
    }
}

void PpiWidget::addRadials(const QVector<asterixqt::Cat240VideoRadial>& radials)
{
    QMutexLocker locker(&mutex_);

    for (const auto& radial : radials) {
        sweepAzimuthDeg_ = (radial.startAzimuthDeg + radial.endAzimuthDeg) / 2.0;

        if (persistentTexture_) {
            if (buffers_[writeBuffer_].isNull()) {
                clearImages();
            }

            if (tripleBuffer_ || doubleBuffer_) {
                if (!buffers_[uploadBuffer_].isNull() && writeBuffer_ != uploadBuffer_) {
                    buffers_[writeBuffer_] = buffers_[uploadBuffer_].copy();
                }
            }

            if (decayEnabled_ && decayTimer_.elapsed() > decayMs_) {
                applyDecay(buffers_[writeBuffer_]);
                decayTimer_.restart();
            }

            rasterizeRadial(radial, buffers_[writeBuffer_]);
            uploadBuffer_ = writeBuffer_;

            if (tripleBuffer_) {
                int next = (writeBuffer_ + 1) % 3;
                if (next == frontBuffer_ || next == uploadBuffer_) {
                    next = (next + 1) % 3;
                }
                writeBuffer_ = next;
            } else if (doubleBuffer_) {
                writeBuffer_ = writeBuffer_ == 0 ? 1 : 0;
            }

            textureDirty_ = true;
        } else {
            radials_.push_back(radial);
        }
    }

    if (radials_.size() > 1024) {
        radials_.erase(radials_.begin(), radials_.begin() + (radials_.size() - 1024));
    }
}

void PpiWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);

    createPrograms();

    vao_.create();
    vbo_.create();
    initTexture();
}

void PpiWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    clearImages();
    initTexture();
}

void PpiWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (persistentTexture_) {
        uploadTexture();
        drawTexture();
    }

    QVector<Vertex> lines;
    QVector<Vertex> points;

    const QPointF c = center();
    const double r = radius();
    const QColor gridColor(0, 90, 0);
    const double stepNm = dynamicRingStepNm();

    for (double nm = stepNm; nm <= rangeNm_ + 0.001; nm += stepNm) {
        QPointF previous(c.x(), c.y() - r * nm / rangeNm_);

        for (int i = 1; i <= 192; ++i) {
            const double angle = 2.0 * M_PI * i / 192.0;
            const QPointF current(c.x() + r * nm / rangeNm_ * std::sin(angle),
                                  c.y() - r * nm / rangeNm_ * std::cos(angle));
            appendLine(lines, previous, current, gridColor);
            previous = current;
        }
    }

    for (int angle = 0; angle < 360; angle += 30) {
        const double angleRad = qDegreesToRadians(double(angle));
        appendLine(lines,
                   c,
                   QPointF(c.x() + r * std::sin(angleRad),
                           c.y() - r * std::cos(angleRad)),
                   gridColor);
    }

    QVector<asterixqt::Cat048Target> targets;
    {
        QMutexLocker locker(&mutex_);
        targets = targets_;
    }

    for (const auto& target : targets) {
        if (target.hasPolar && target.rangeNm <= rangeNm_) {
            const QPointF targetPoint = polar(target.azimuthDeg, target.rangeNm);

            if (target.hasTrackNumber) {
                const double size = 7.0;
                const QColor trackColor(255, 0, 0);

                const QPointF top(targetPoint.x(), targetPoint.y() - size);
                const QPointF right(targetPoint.x() + size, targetPoint.y());
                const QPointF bottom(targetPoint.x(), targetPoint.y() + size);
                const QPointF left(targetPoint.x() - size, targetPoint.y());

                appendLine(lines, top, right, trackColor);
                appendLine(lines, right, bottom, trackColor);
                appendLine(lines, bottom, left, trackColor);
                appendLine(lines, left, top, trackColor);
            } else {
                appendPoint(points, targetPoint, QColor(0, 255, 0));
            }
        }
    }

    const double sweepRad = qDegreesToRadians(sweepAzimuthDeg_);
    appendLine(lines,
               c,
               QPointF(c.x() + r * std::sin(sweepRad),
                       c.y() - r * std::cos(sweepRad)),
               QColor(0, 255, 0));

    draw(GL_LINES, lines);
    draw(GL_POINTS, points, 5.0f);
    drawOverlay();
}

void PpiWidget::wheelEvent(QWheelEvent* event)
{
    if (event->angleDelta().y() > 0) {
        zoomIn();
    } else {
        zoomOut();
    }

    event->accept();
}

void PpiWidget::mouseMoveEvent(QMouseEvent* event)
{
    lastMousePosition_ = event->pos();
    mousePosition_ = mouseToPpi(lastMousePosition_);
    update();
}

void PpiWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    lastMousePosition_ = event->pos();
    mousePosition_ = mouseToPpi(lastMousePosition_);
    selectedTargetIndex_ = findNearestTarget(lastMousePosition_);

    QVector<asterixqt::Cat048Target> targets;
    {
        QMutexLocker locker(&mutex_);
        targets = targets_;
    }

    selectedIsTrack_ = false;
    selectedTrackNumber_ = 0;

    if (selectedTargetIndex_ >= 0 && selectedTargetIndex_ < targets.size()) {
        const auto& selected = targets[selectedTargetIndex_];
        if (selected.hasTrackNumber) {
            selectedIsTrack_ = true;
            selectedTrackNumber_ = selected.trackNumber;
        }
    }

    update();
}

QPointF PpiWidget::center() const
{
    return QPointF(width() / 2.0, height() / 2.0);
}

double PpiWidget::radius() const
{
    return qMin(width(), height()) * 0.46 * zoom_;
}

QPointF PpiWidget::polar(double azimuthDeg, double rangeNm) const
{
    const double scale = radius() / rangeNm_;
    const QPointF c = center();
    const double azimuthRad = qDegreesToRadians(azimuthDeg);

    return QPointF(c.x() + rangeNm * scale * std::sin(azimuthRad),
                   c.y() - rangeNm * scale * std::cos(azimuthRad));
}

PpiWidget::MousePpiPosition PpiWidget::mouseToPpi(const QPoint& mousePosition) const
{
    MousePpiPosition result;
    const QPointF c = center();
    const double dxPixels = mousePosition.x() - c.x();
    const double dyPixels = c.y() - mousePosition.y();
    const double distancePixels = std::sqrt(dxPixels * dxPixels + dyPixels * dyPixels);

    if (distancePixels > radius()) {
        return result;
    }

    result.valid = true;
    result.rangeNm = distancePixels * rangeNm_ / radius();
    result.rangeMeters = result.rangeNm * MetersPerNm;

    double azimuth = qRadiansToDegrees(std::atan2(dxPixels, dyPixels));
    if (azimuth < 0.0) {
        azimuth += 360.0;
    }

    result.azimuthDeg = azimuth;
    result.xMeters = result.rangeMeters * std::sin(qDegreesToRadians(result.azimuthDeg));
    result.yMeters = result.rangeMeters * std::cos(qDegreesToRadians(result.azimuthDeg));

    return result;
}

int PpiWidget::findNearestTarget(const QPoint& mousePosition, double maxDistancePixels) const
{
    QVector<asterixqt::Cat048Target> targets;
    {
        QMutexLocker locker(&mutex_);
        targets = targets_;
    }

    int bestIndex = -1;
    double bestDistance = maxDistancePixels;

    for (int i = 0; i < targets.size(); ++i) {
        const auto& target = targets[i];
        if (!target.hasPolar || target.rangeNm > rangeNm_) {
            continue;
        }

        const QPointF targetPoint = polar(target.azimuthDeg, target.rangeNm);
        const double dx = targetPoint.x() - mousePosition.x();
        const double dy = targetPoint.y() - mousePosition.y();
        const double distance = std::sqrt(dx * dx + dy * dy);

        if (distance < bestDistance) {
            bestDistance = distance;
            bestIndex = i;
        }
    }

    return bestIndex;
}

int PpiWidget::findTargetByTrackNumber(quint16 trackNumber) const
{
    QVector<asterixqt::Cat048Target> targets;
    {
        QMutexLocker locker(&mutex_);
        targets = targets_;
    }

    for (int i = 0; i < targets.size(); ++i) {
        if (targets[i].hasTrackNumber && targets[i].trackNumber == trackNumber) {
            return i;
        }
    }

    return -1;
}

double PpiWidget::dynamicRingStepNm() const
{
    if (rangeNm_ <= 2.0) {
        return 0.25;
    }

    if (rangeNm_ <= 5.0) {
        return 0.5;
    }

    if (rangeNm_ <= 10.0) {
        return 1.0;
    }

    if (rangeNm_ <= 15.0) {
        return 2.0;
    }

    return 5.0;
}

void PpiWidget::createPrograms()
{
    const char* vertexShader = R"(
        #version 330 core
        layout(location = 0) in vec2 p;
        layout(location = 1) in vec4 c;
        uniform vec2 vp;
        uniform float ps;
        out vec4 vc;
        void main()
        {
            vec2 n = vec2((p.x / vp.x) * 2.0 - 1.0,
                          1.0 - (p.y / vp.y) * 2.0);
            gl_Position = vec4(n, 0.0, 1.0);
            gl_PointSize = ps;
            vc = c;
        }
    )";

    const char* fragmentShader = R"(
        #version 330 core
        in vec4 vc;
        out vec4 f;
        void main()
        {
            f = vc;
        }
    )";

    colorProgram_.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader);
    colorProgram_.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader);
    colorProgram_.link();

    const char* textureVertexShader = R"(
        #version 330 core
        layout(location = 0) in vec2 p;
        layout(location = 1) in vec2 t;
        out vec2 vt;
        void main()
        {
            gl_Position = vec4(p, 0.0, 1.0);
            vt = t;
        }
    )";

    const char* textureFragmentShader = R"(
        #version 330 core
        in vec2 vt;
        uniform sampler2D tex;
        out vec4 f;
        void main()
        {
            f = texture(tex, vt);
        }
    )";

    textureProgram_.addShaderFromSourceCode(QOpenGLShader::Vertex, textureVertexShader);
    textureProgram_.addShaderFromSourceCode(QOpenGLShader::Fragment, textureFragmentShader);
    textureProgram_.link();
}

void PpiWidget::initTexture()
{
    if (!textureId_) {
        glGenTextures(1, &textureId_);
    }

    glBindTexture(GL_TEXTURE_2D, textureId_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (buffers_[frontBuffer_].isNull()) {
        clearImages();
    }

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 width(),
                 height(),
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 buffers_[frontBuffer_].constBits());

    textureDirty_ = false;
}

void PpiWidget::clearImages()
{
    if (width() <= 0 || height() <= 0) {
        return;
    }

    for (auto& image : buffers_) {
        image = QImage(width(), height(), QImage::Format_RGBA8888);
        image.fill(Qt::transparent);
    }

    frontBuffer_ = 0;
    uploadBuffer_ = 1;
    writeBuffer_ = 2;
    textureDirty_ = true;
}

void PpiWidget::applyDecay(QImage& image)
{
    for (int y = 0; y < image.height(); ++y) {
        auto* pixels = reinterpret_cast<QRgb*>(image.scanLine(y));

        for (int x = 0; x < image.width(); ++x) {
            QColor color = QColor::fromRgba(pixels[x]);
            if (color.alpha() == 0) {
                continue;
            }

            color.setGreen(int(color.green() * 0.88));
            color.setAlpha(int(color.alpha() * 0.82));
            pixels[x] = color.rgba();
        }
    }
}

void PpiWidget::rasterizeRadial(const asterixqt::Cat240VideoRadial& radial,
                                QImage& image)
{
    if (radial.amplitudes.isEmpty()) {
        return;
    }

    const double azimuth = (radial.startAzimuthDeg + radial.endAzimuthDeg) / 2.0;
    const int step = qMax(1, radial.amplitudes.size() / MaxVideoCellsPerRadial);

    for (int i = 0; i < radial.amplitudes.size(); i += step) {
        const double rangeNm = radial.cellDurationSeconds
            * double(radial.startRangeCell + quint32(i))
            * LightSpeedMps
            / 2.0
            / MetersPerNm;

        if (rangeNm > rangeNm_) {
            continue;
        }

        const QPointF point = polar(azimuth, rangeNm);
        const int x = qRound(point.x());
        const int y = qRound(point.y());

        if (x >= 0 && y >= 0 && x < image.width() && y < image.height()) {
            image.setPixelColor(x,
                                y,
                                QColor(0,
                                       qBound(0, int(radial.amplitudes[i]) * 4, 240),
                                       0,
                                       170));
        }
    }
}

void PpiWidget::uploadTexture()
{
    if (!textureDirty_) {
        return;
    }

    frontBuffer_ = uploadBuffer_;

    glBindTexture(GL_TEXTURE_2D, textureId_);
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    0,
                    0,
                    width(),
                    height(),
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    buffers_[frontBuffer_].constBits());

    textureDirty_ = false;
}

void PpiWidget::drawTexture()
{
    struct TextureVertex
    {
        float x;
        float y;
        float u;
        float v;
    };

    TextureVertex vertices[] = {
        {-1.0f, -1.0f, 0.0f, 1.0f},
        { 1.0f, -1.0f, 1.0f, 1.0f},
        { 1.0f,  1.0f, 1.0f, 0.0f},
        {-1.0f, -1.0f, 0.0f, 1.0f},
        { 1.0f,  1.0f, 1.0f, 0.0f},
        {-1.0f,  1.0f, 0.0f, 0.0f}
    };

    vao_.bind();
    vbo_.bind();
    vbo_.allocate(vertices, sizeof(vertices));

    textureProgram_.bind();
    glBindTexture(GL_TEXTURE_2D, textureId_);
    textureProgram_.setUniformValue("tex", 0);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(TextureVertex),
                          reinterpret_cast<void*>(2 * sizeof(float)));

    glDrawArrays(GL_TRIANGLES, 0, 6);

    textureProgram_.release();
    vbo_.release();
    vao_.release();
}

void PpiWidget::appendLine(QVector<Vertex>& vertices,
                           QPointF a,
                           QPointF b,
                           QColor color) const
{
    vertices.push_back({float(a.x()), float(a.y()), float(color.redF()), float(color.greenF()), float(color.blueF()), float(color.alphaF())});
    vertices.push_back({float(b.x()), float(b.y()), float(color.redF()), float(color.greenF()), float(color.blueF()), float(color.alphaF())});
}

void PpiWidget::appendPoint(QVector<Vertex>& vertices,
                            QPointF point,
                            QColor color) const
{
    vertices.push_back({float(point.x()), float(point.y()), float(color.redF()), float(color.greenF()), float(color.blueF()), float(color.alphaF())});
}

void PpiWidget::draw(GLenum primitive,
                     const QVector<Vertex>& vertices,
                     float pointSize)
{
    if (vertices.isEmpty()) {
        return;
    }

    vao_.bind();
    vbo_.bind();
    vbo_.allocate(vertices.constData(), vertices.size() * int(sizeof(Vertex)));

    colorProgram_.bind();
    colorProgram_.setUniformValue("vp", QVector2D(width(), height()));
    colorProgram_.setUniformValue("ps", pointSize);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          reinterpret_cast<void*>(2 * sizeof(float)));

    glDrawArrays(primitive, 0, vertices.size());

    colorProgram_.release();
    vbo_.release();
    vao_.release();
}

void PpiWidget::drawOverlay()
{
    QPainter painter(this);
    painter.setPen(QColor(0, 230, 0));

    const QPointF c = center();
    const double r = radius();
    const double stepNm = dynamicRingStepNm();

    for (double nm = stepNm; nm <= rangeNm_ + 0.001; nm += stepNm) {
        const double ringRadius = r * nm / rangeNm_;
        painter.drawText(QPointF(c.x() + 6.0, c.y() - ringRadius - 4.0),
                         QString("%1 NM").arg(nm, 0, 'f', stepNm < 1.0 ? 2 : 0));
    }

    painter.drawText(10,
                     20,
                     QString("Range %1 NM | Step %2 NM | FPS %3 | Triple %4 | Decay %5ms")
                         .arg(rangeNm_, 0, 'f', rangeNm_ < 10 ? 1 : 0)
                         .arg(stepNm, 0, 'f', stepNm < 1.0 ? 2 : 0)
                         .arg(frameRate_)
                         .arg(tripleBuffer_ ? "ON" : "OFF")
                         .arg(decayEnabled_ ? QString::number(decayMs_) : "OFF"));

    int overlayY = 42;
    if (mousePosition_.valid) {
        painter.drawText(10,
                         overlayY,
                         QString("Mouse: R %1 m / %2 NM | Az %3 deg | X %4 m | Y %5 m")
                             .arg(mousePosition_.rangeMeters, 0, 'f', 1)
                             .arg(mousePosition_.rangeNm, 0, 'f', 3)
                             .arg(mousePosition_.azimuthDeg, 0, 'f', 2)
                             .arg(mousePosition_.xMeters, 0, 'f', 1)
                             .arg(mousePosition_.yMeters, 0, 'f', 1));
    } else {
        painter.drawText(10, overlayY, QStringLiteral("Mouse: fuori PPI"));
    }

    overlayY += 22;

    QVector<asterixqt::Cat048Target> targets;
    {
        QMutexLocker locker(&mutex_);
        targets = targets_;
    }

    int overlaySelectionIndex = selectedTargetIndex_;
    if (selectedIsTrack_) {
        for (int i = 0; i < targets.size(); ++i) {
            if (targets[i].hasTrackNumber && targets[i].trackNumber == selectedTrackNumber_) {
                overlaySelectionIndex = i;
                break;
            }
        }
    }

    if (overlaySelectionIndex >= 0 && overlaySelectionIndex < targets.size()) {
        const auto& target = targets[overlaySelectionIndex];
        const double rangeMeters = target.rangeNm * MetersPerNm;

        painter.setPen(QColor(255, 255, 0));
        painter.drawText(10,
                         overlayY,
                         QString("Selected %1 | SAC/SIC %2/%3 | Track %4")
                             .arg(target.hasTrackNumber ? "Track" : "Plot")
                             .arg(target.sac)
                             .arg(target.sic)
                             .arg(target.hasTrackNumber ? QString::number(target.trackNumber) : "N/A"));

        overlayY += 18;
        painter.drawText(10,
                         overlayY,
                         QString("R %1 m / %2 NM | Az %3 deg | GS %4 kt | HDG %5 deg")
                             .arg(rangeMeters, 0, 'f', 1)
                             .arg(target.rangeNm, 0, 'f', 3)
                             .arg(target.azimuthDeg, 0, 'f', 2)
                             .arg(target.hasVelocity ? QString::number(target.groundSpeedKt, 'f', 1) : "N/A")
                             .arg(target.hasVelocity ? QString::number(target.headingDeg, 'f', 1) : "N/A"));

        painter.drawEllipse(polar(target.azimuthDeg, target.rangeNm), 8.0, 8.0);
    }
}
