#include "GLWindow.h"

#include <QKeyEvent>
#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <iostream>
#include <QDateTime>
#include <qmath.h>
QVector3D cameraPos   = QVector3D(0.0f, 0.0f, 3.0f);
QVector3D cameraFront = QVector3D(0.0f, 0.0f, -1.0f);
QVector3D cameraUp    = QVector3D(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

// timing
int deltaTime = 0.0f;	// time between current frame and last frame
int lastFrame = 0.0f;

GLWindow::GLWindow(QWindow *parent)
    : QWindow(parent)
{
    setSurfaceType(QSurface::OpenGLSurface);

    QVector3D a(0, 0, 1);
    QVector3D b(1,0, 0);
}

GLWindow::~GLWindow()
{
    if( m_device )
        delete m_device;
}

//! [2]
void GLWindow::render(QPainter *painter)
{
    Q_UNUSED(painter);
}
QVector3D cubePositions[] = {
    QVector3D( 0.0f,  0.0f,  0.0f),
    QVector3D( 2.0f,  5.0f, -15.0f),
    QVector3D(-1.5f, -2.2f, -2.5f),
    QVector3D(-3.8f, -2.0f, -12.3f),
    QVector3D( 2.4f, -0.4f, -3.5f),
    QVector3D(-1.7f,  3.0f, -7.5f),
    QVector3D( 1.3f, -2.0f, -2.5f),
    QVector3D( 1.5f,  2.0f, -2.5f),
    QVector3D( 1.5f,  0.2f, -1.5f),
    QVector3D(-1.3f,  1.0f, -1.5f)
};

float planeMulti = 2;

float planeVertices[] = {
    // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
    5.0f * planeMulti, -0.5f,  5.0f * planeMulti,  2.0f * planeMulti, 0.0f,
    -5.0f * planeMulti, -0.5f,  5.0f * planeMulti,  0.0f, 0.0f,
    -5.0f * planeMulti, -0.5f, -5.0f * planeMulti,  0.0f, 2.0f * planeMulti,

    5.0f * planeMulti, -0.5f,  5.0f * planeMulti,  2.0f * planeMulti, 0.0f,
    -5.0f * planeMulti, -0.5f, -5.0f * planeMulti,  0.0f, 2.0f * planeMulti,
    5.0f * planeMulti, -0.5f, -5.0f * planeMulti,  2.0f * planeMulti, 2.0f * planeMulti
};

float vegetationVertices[] = {
    -0.5f, 0.50f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.0f,0.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
};

float cubeVertices[] = {
    // positions          // texture Coords
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

QVector<QVector3D> vegetation;


void GLWindow::initialize()
{
    vegetation.push_back(QVector3D(-1.5f,  0.1f, -0.48f));
    vegetation.push_back(QVector3D( 1.5f,  0.1f,  0.51f));
    vegetation.push_back(QVector3D( 0.0f,  0.1f,  0.7f));
    vegetation.push_back(QVector3D(-0.3f,  0.1f, -2.3f));
    vegetation.push_back(QVector3D( 0.5f,  0.1f, -0.6f));
    // build and compile our shader program
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/glsl/primitive.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/glsl/primitive.frag");
    m_program->link();

    // set up vertex data (and buffer(s)) and configure vertex attributes
    m_vao_cube = new QOpenGLVertexArrayObject;
    m_vao_cube->create();
    m_vao_cube->bind();
    m_vbo_cube = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vbo_cube->create();
    m_vbo_cube->bind();
    m_vbo_cube->allocate(cubeVertices, sizeof(cubeVertices));

    m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    m_program->setAttributeBuffer(2, GL_FLOAT, 3 * sizeof(float), 2, 5 * sizeof(float));
    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(2);

    m_vao_plane = new QOpenGLVertexArrayObject;
    m_vao_plane->create();
    m_vao_plane->bind();
    m_vbo_plane= new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vbo_plane->create();
    m_vbo_plane->bind();
    m_vbo_plane->allocate(planeVertices, sizeof(planeVertices));
    m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    m_program->setAttributeBuffer(2, GL_FLOAT, 3 * sizeof(float), 2, 5 * sizeof(float));
    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(2);

    m_vao_vertical= new QOpenGLVertexArrayObject;
    m_vao_vertical->create();
    m_vao_vertical->bind();
    m_vbo_vertical = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vbo_vertical->create();
    m_vbo_vertical->bind();
    m_vbo_vertical->allocate(vegetationVertices, sizeof(vegetationVertices));
    m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    m_program->setAttributeBuffer(2, GL_FLOAT, 3 * sizeof(float), 2, 5 * sizeof(float));
    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(2);

    m_tex_cube = new QOpenGLTexture(QImage(":/img/container.jpg").mirrored());
    m_tex_plane = new QOpenGLTexture(QImage(":/img/awesomeface.png").mirrored());
    m_tex_glass = new QOpenGLTexture(QImage(":/img/blending_transparent_window.png").mirrored());
    glActiveTexture(GL_TEXTURE0);
    m_tex_cube->bind();
    glActiveTexture(GL_TEXTURE1);
    m_tex_plane->bind();
    glActiveTexture(GL_TEXTURE2);
    m_tex_glass->bind();

    m_model.setToIdentity();
    m_view.setToIdentity();
    m_project.setToIdentity();


}



#include <map>
#include <iostream>
using namespace std;

void GLWindow::render()
{


    int currentFrame = (int)(QDateTime::currentMSecsSinceEpoch());
    //qDebug() << currentFrame;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    map<float, QVector3D> dists;
    for(int i = 0; i < vegetation.size(); i++ )
    {
        float len = vegetation[i].distanceToPoint(cameraPos);
        dists[len] = vegetation[i];
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable( GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // draw our first triangle
    m_view.setToIdentity();
    m_view.lookAt(cameraPos, cameraPos + cameraFront, cameraUp );

    m_project.setToIdentity();
    m_project.perspective(fov, 900.f/600.f, 0.1f, 100.0f);

    m_program->bind();
    m_program->setUniformValue("projection", m_project);
    m_program->setUniformValue("view",  m_view);

    m_vao_cube->bind();
    m_program->setUniformValue("texture1", 0);
    m_model.setToIdentity();
    m_model.translate(QVector3D(-1.0f, 0.1f, -3.0f) );
    m_program->setUniformValue("model", m_model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    m_model.setToIdentity();
    m_model.translate(QVector3D(2.0f, 0.1f, -2.0f) );
    m_program->setUniformValue("model", m_model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    m_vao_cube->release();

    m_vao_plane->bind();
    m_model.setToIdentity();
    m_program->setUniformValue("texture1", 1);
    m_program->setUniformValue("model", m_model);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    m_vao_vertical->bind();
    m_program->setUniformValue("texture1", 2);

    for( map<float,QVector3D>::reverse_iterator it = dists.rbegin(); it != dists.rend(); ++it )
    {
        m_model.setToIdentity();
        m_model.translate( it->second);
        m_program->setUniformValue("model", m_model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }


    m_program->release();
}
//! [2]

//! [3]
void GLWindow::renderLater()
{
    requestUpdate();
}

bool GLWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

void GLWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}

void GLWindow::keyPressEvent(QKeyEvent *event)
{
    QKeyEvent* keyEvent = (QKeyEvent*)event;

    float cameraSpeed = static_cast<float>(0.025 * deltaTime);

    if( keyEvent->key() == 0x1000013 )
    {
        m_alpha += 0.1f;
        if( m_alpha >= 1.00f ) m_alpha = 1.00f;
    }
    else if(keyEvent->key() == 0x1000015 )
    {
        m_alpha -= 0.1f;
        if(m_alpha <= 0.00f ) m_alpha = 0.00f;
    }else if(keyEvent->text() == 'w')
    {
        cameraPos += cameraSpeed * cameraFront;
    }else if(keyEvent->text() == 's')
    {
        cameraPos -= cameraSpeed * cameraFront;
    }else if(keyEvent->text() == 'a')
    {
        cameraPos -= (QVector3D::crossProduct(cameraFront, cameraUp)).normalized() * cameraSpeed;
    }else if(keyEvent->text() == 'd')
    {
        cameraPos += (QVector3D::crossProduct(cameraFront, cameraUp)).normalized() * cameraSpeed;
    }else if(keyEvent->text() == '4')
    {
        firstPerson(-20, 0);
    }
    else if(keyEvent->text() == '6')
    {
        firstPerson(20, 0);
    }
}

void GLWindow::mouseMoveEvent(QMouseEvent *event)
{

    float xpos = static_cast<float>(event->pos().x());
    float ypos = static_cast<float>(event->pos().y());

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    firstPerson(xoffset, yoffset);
}

void GLWindow::resizeEvent(QResizeEvent *event)
{
    qDebug() << event->oldSize() << " -> " << event->size();
    float width = event->size().width();
    float height = event->size().height();
    if(m_context != nullptr )
        glViewport(0, 0, width, height);
}


//! [3]

//! [4]
void GLWindow::renderNow()
{
    if (!isExposed())
        return;

    bool needsInitialize = false;

    if (!m_context) {
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());
        m_context->create();

        needsInitialize = true;
    }

    m_context->makeCurrent(this);

    if (needsInitialize) {
        initializeOpenGLFunctions();
        initialize();
    }

    render();

    m_context->swapBuffers(this);

    if (m_animating)
        renderLater();
}
//! [4]

//! [5]
void GLWindow::setAnimating(bool animating)
{
    m_animating = animating;

    if (animating)
        renderLater();
}
//! [5]

void GLWindow::firstPerson(float xoffset, float yoffset)
{
    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    QVector3D front;
    front.setX( qCos(qDegreesToRadians(yaw)) * qCos(qDegreesToRadians(pitch)) );
    front.setY( qSin(qDegreesToRadians(pitch)) );
    front.setZ(qSin(qDegreesToRadians(yaw)) * qCos(qDegreesToRadians(pitch)));
    cameraFront = front.normalized();
}
