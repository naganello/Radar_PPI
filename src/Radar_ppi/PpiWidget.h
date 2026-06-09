#pragma once

#include <AsterixQt/AsterixTypes.h>

#include <QElapsedTimer>
#include <QImage>
#include <QMouseEvent>
#include <QMutex>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QTimer>
#include <QWheelEvent>

class PpiWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit PpiWidget(QWidget* parent = nullptr);
    ~PpiWidget() override;

    int frameRate() const;

public slots:
    void addTargets(const QVector<asterixqt::Cat048Target>& targets);
    void addRadials(const QVector<asterixqt::Cat240VideoRadial>& radials);

    void setFrameRate(int fps);
    void setRangeNm(double rangeNm);
    void zoomIn();
    void zoomOut();
    void resetView();
    void clear();

    void setPersistentTextureEnabled(bool enabled);
    void setDoubleBufferEnabled(bool enabled);
    void setTripleBufferEnabled(bool enabled);
    void setSweepDecayEnabled(bool enabled);
    void setSweepDecayMs(int decayMs);

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;
    void wheelEvent(QWheelEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    struct Vertex
    {
        float x = 0.0f;
        float y = 0.0f;
        float r = 0.0f;
        float g = 1.0f;
        float b = 0.0f;
        float a = 1.0f;
    };

    struct MousePpiPosition
    {
        bool valid = false;
        double rangeNm = 0.0;
        double rangeMeters = 0.0;
        double azimuthDeg = 0.0;
        double xMeters = 0.0;
        double yMeters = 0.0;
    };

    QPointF center() const;
    double radius() const;
    QPointF polar(double azimuthDeg, double rangeNm) const;
    MousePpiPosition mouseToPpi(const QPoint& mousePosition) const;
    int findNearestTarget(const QPoint& mousePosition, double maxDistancePixels = 12.0) const;
    double dynamicRingStepNm() const;

    void createPrograms();
    void initTexture();
    void clearImages();
    void applyDecay(QImage& image);
    void rasterizeRadial(const asterixqt::Cat240VideoRadial& radial, QImage& image);
    void uploadTexture();
    void drawTexture();

    void appendLine(QVector<Vertex>& vertices,
                    QPointF a,
                    QPointF b,
                    QColor color) const;
    void appendPoint(QVector<Vertex>& vertices,
                     QPointF point,
                     QColor color) const;
    void draw(GLenum primitive,
              const QVector<Vertex>& vertices,
              float pointSize = 1.0f);
    void drawOverlay();

    QOpenGLShaderProgram colorProgram_;
    QOpenGLShaderProgram textureProgram_;
    QOpenGLBuffer vbo_;
    QOpenGLVertexArrayObject vao_;
    GLuint textureId_ = 0;

    QTimer repaintTimer_;
    int frameRate_ = 30;

    mutable QMutex mutex_;
    QVector<asterixqt::Cat048Target> targets_;
    QVector<asterixqt::Cat240VideoRadial> radials_;

    QImage buffers_[3];
    int frontBuffer_ = 0;
    int uploadBuffer_ = 1;
    int writeBuffer_ = 2;

    bool persistentTexture_ = true;
    bool doubleBuffer_ = false;
    bool tripleBuffer_ = true;
    bool decayEnabled_ = true;
    bool textureDirty_ = true;

    int decayMs_ = 80;
    QElapsedTimer decayTimer_;

    double rangeNm_ = 25.0;
    double zoom_ = 1.0;
    double sweepAzimuthDeg_ = 0.0;

    QPoint lastMousePosition_;
    MousePpiPosition mousePosition_;
    int selectedTargetIndex_ = -1;
};
