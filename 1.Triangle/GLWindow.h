#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <QWindow>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
// #include <eigen3/Eigen/Eigen>
#include <QMatrix4x4>
#include <QMouseEvent>
QT_BEGIN_NAMESPACE
class QPainter;
class QOpenGLContext;
class QOpenGLPaintDevice;
QT_END_NAMESPACE

class GLWindow : public QWindow, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    GLWindow(QWindow *parent = nullptr);
    ~GLWindow();
    virtual void render(QPainter *painter);
    virtual void render();
    virtual void initialize();
    void setAnimating(bool animating);

public slots:
    void renderLater();
    void renderNow();

protected:
    bool event(QEvent *event) override;
    void exposeEvent(QExposeEvent *event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    bool m_animating = false;

    QOpenGLContext *m_context = nullptr;
    QOpenGLPaintDevice *m_device = nullptr;
    QOpenGLShaderProgram* m_program = nullptr;
    QOpenGLBuffer* m_vbo = nullptr;
    QOpenGLBuffer* m_ebo = nullptr;
    QOpenGLTexture* m_tex_container = nullptr;
    QOpenGLTexture* m_tex_thumb = nullptr;
private:
    float m_alpha = 0.0f;
private:
    QMatrix4x4 m_model;
    QMatrix4x4 m_view;
    QMatrix4x4 m_project;
};
#endif // GLWINDOW_H
