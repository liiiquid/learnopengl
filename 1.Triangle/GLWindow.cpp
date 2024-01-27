#include "GLWindow.h"

#include <QKeyEvent>
#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <iostream>
#include <QDateTime>
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
void GLWindow::initialize()
{
    // build and compile our shader program
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/glsl/primitive.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/glsl/primitive.frag");
    m_program->link();

    // set up vertex data (and buffer(s)) and configure vertex attributes
    float vertices[] = {
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

    for(int i = 0; i < sizeof(vertices)/ (5 * sizeof(float)); i++)
    {
        // vertices[i * 5 + 0] *= -1;
        // vertices[i * 5 + 1] *= -1;
        // vertices[i * 5 + 2] *= -1;
    }

    static const unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vbo->create();
    m_vbo->bind();
    m_vbo->allocate(vertices, sizeof(vertices));

    m_ebo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_ebo->create();
    m_ebo->bind();
    m_ebo->allocate(indices, sizeof(indices));

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    m_tex_container = new QOpenGLTexture(QImage(":/img/container.jpg").mirrored());
    m_tex_thumb = new QOpenGLTexture(QImage(":/img/awesomeface.png").mirrored());
    glActiveTexture(GL_TEXTURE0);
    m_tex_container->bind();
    glActiveTexture(GL_TEXTURE1);
    m_tex_thumb->bind();
    m_program->bind();
    m_program->setUniformValue("tex_container", 0 );
    m_program->setUniformValue("tex_thumb", 1);

    m_alpha = 0;
    m_program->setUniformValue("alpha", m_alpha);

    m_model.setToIdentity();
    m_view.setToIdentity();
    m_project.setToIdentity();

    m_program->setUniformValue("mvp", m_project * m_view * m_model);
}

void GLWindow::render()
{

    int currentFrame = (int)(QDateTime::currentMSecsSinceEpoch());
    //qDebug() << currentFrame;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    m_program->bind();
    m_vbo->bind();
    m_ebo->bind();


    m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    //m_program->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 3, 8 * sizeof(GLfloat));
    m_program->setAttributeBuffer(2, GL_FLOAT, 3 * sizeof(float), 2, 5 * sizeof(float));
    m_program->enableAttributeArray(0);
    //m_program->enableAttributeArray(1);
    m_program->enableAttributeArray(2);

    m_program->setUniformValue("alpha", m_alpha);


    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width(), height());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // draw our first triangle
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    m_view.setToIdentity();
    m_view.lookAt(cameraPos, cameraPos + cameraFront, cameraUp );

    m_project.setToIdentity();
    m_project.perspective(fov, 900.f/600.f, 0.1f, 100.0f);

    for(int i = 0; i < 10; i++)
    {
        m_model.setToIdentity();
        m_model.scale(1, 1, 1);
        m_model.translate(cubePositions[i]);
        float angle = 20.0f * i;
        m_model.rotate(angle, QVector3D(1, 0.3, 0.5));
        m_program->setUniformValue("mvp", m_project * m_view * m_model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
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
    qDebug() << keyEvent->text() <<  ", " << keyEvent->key();
    float cameraSpeed = static_cast<float>(0.025 * deltaTime);
    qDebug() << cameraSpeed;

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
    }
}

void GLWindow::mouseMoveEvent(QMouseEvent *event)
{

    float xpos = static_cast<float>(event->pos().x());
    float ypos = static_cast<float>(event->pos().y());

    // if (firstMouse)
    // {
    //     lastX = xpos;
    //     lastY = ypos;
    //     //firstMouse = false;
    // }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.001f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    // if (pitch > 89.0f)
    //     pitch = 89.0f;
    // if (pitch < -89.0f)
    //     pitch = -89.0f;

    QVector3D front;
    front.setX( cos(yaw) * cos(pitch) );
    front.setY( sin(pitch) );
    front.setZ(sin(yaw) * cos(pitch));
    cameraFront = front.normalized();
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

