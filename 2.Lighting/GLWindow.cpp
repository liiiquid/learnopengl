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
// lighting
QVector3D lightPos(1.2f, 1.0f, 2.0f);
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
}

GLWindow::~GLWindow()
{
    if( m_device )
        delete m_device;
}

float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};
QVector3D pointLightPositions[] = {
    QVector3D( 0.7f,  0.2f,  2.0f),
    QVector3D( 2.3f, -3.3f, -4.0f),
    QVector3D(-4.0f,  2.0f, -12.0f),
    QVector3D( 0.0f,  0.0f, -3.0f)
};
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

//! [2]
void GLWindow::render(QPainter *painter)
{
    Q_UNUSED(painter);
}
void GLWindow::initialize()
{
    // build and compile our shader program
    m_lightingProgram = new QOpenGLShaderProgram(this);
    m_lightingProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/glsl/lighting.vert");
    m_lightingProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/glsl/lighting.frag");
    m_lightingProgram->link();

    m_lightProgram = new QOpenGLShaderProgram(this);
    m_lightProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/glsl/lightCube.vert");
    m_lightProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/glsl/lightCube.frag");
    m_lightProgram->link();

    m_vbo_cube = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vbo_cube->create();
    m_vbo_cube->bind();
    m_vbo_cube->allocate(vertices, sizeof(vertices));

    m_tex_container_diffuse = new QOpenGLTexture(QImage(":/img/container2.png").mirrored());

    m_tex_container_specular = new QOpenGLTexture(QImage(":/img/container2_specular.png").mirrored());

    m_tex_spotLight_diffuse = new QOpenGLTexture(QImage(":/img/Vpzmue9.jpeg").mirrored());
    glActiveTexture(GL_TEXTURE0);
    m_tex_container_diffuse->bind();
    glActiveTexture(GL_TEXTURE1);
    m_tex_container_specular->bind();
    glActiveTexture(GL_TEXTURE2);
    m_tex_spotLight_diffuse->bind();

    m_lightingProgram->bind();
    m_lightingProgram->setAttributeBuffer("aPos",    GL_FLOAT, 0, 3, 8 * sizeof(GLfloat));
    m_lightingProgram->setAttributeBuffer("aNormal", GL_FLOAT, 3 * sizeof(GLfloat), 3, 8 * sizeof(GLfloat));
    m_lightingProgram->setAttributeBuffer("aTexCoords", GL_FLOAT, 6 * sizeof(GLfloat), 2, 8 * sizeof(GLfloat));
    m_lightingProgram->enableAttributeArray("aPos");
    m_lightingProgram->enableAttributeArray("aNormal");
    m_lightingProgram->enableAttributeArray("aTexCoords");

    m_lightingProgram->setUniformValue("material.shininess", 32.0f);
    m_lightingProgram->setUniformValue("material.diffuse", 0);
    m_lightingProgram->setUniformValue("material.specular", 1);

    m_lightProgram->setUniformValue("materialflash.shininess", 32.0f);
    m_lightingProgram->setUniformValue("materialflash.diffuse",2);
    m_lightingProgram->setUniformValue("materialflash.specular", 2);

    m_lightingProgram->setUniformValue("spotLight.ambient", QVector3D(0.1f, 0.1f, 0.1f));
    m_lightingProgram->setUniformValue("spotLight.diffuse", QVector3D(0.8f, 0.8f, 0.8f));
    m_lightingProgram->setUniformValue("spotLight.specular", QVector3D(1.0f, 1.0f, 1.0f));
    m_lightingProgram->setUniformValue("spotLight.cutOff", (float)qCos(qDegreesToRadians(12.5f) ));
    m_lightingProgram->setUniformValue("spotLight.outerCutOff", (float)qCos(qDegreesToRadians(17.5f)));
    m_lightingProgram->setUniformValue("spotLight.constant",  1.0f);
    m_lightingProgram->setUniformValue("spotLight.linear",    0.09f);
    m_lightingProgram->setUniformValue("spotLight.quadratic", 0.032f);
    QString pointLights = "pointLights[%1]";
    for(int i = 0; i < 4; i++ )
    {
        pointLights = pointLights.arg(i);
        m_lightingProgram->setUniformValue( (pointLights+".ambient").toLocal8Bit().data(), QVector3D(0.1f, 0.1f, 0.1f));
        qDebug() << (pointLights+".ambient").toLocal8Bit().data();
        m_lightingProgram->setUniformValue((pointLights+".diffuse").toLocal8Bit().data(), QVector3D(0.8f, 0.8f, 0.8f));
        m_lightingProgram->setUniformValue((pointLights+".specular").toLocal8Bit().data(), QVector3D(1.0f, 1.0f, 1.0f));
        m_lightingProgram->setUniformValue((pointLights+".constant").toLocal8Bit().data(),  1.0f);
        m_lightingProgram->setUniformValue((pointLights+".linear").toLocal8Bit().data(),    0.09f);
        m_lightingProgram->setUniformValue((pointLights+".quadratic").toLocal8Bit().data(), 0.032f);
        m_lightingProgram->setUniformValue((pointLights+".position").toLocal8Bit().data(), pointLightPositions[i]);
        pointLights = "pointLights[%1]";
    }

    m_lightingProgram->setUniformValue("directLight.ambient", QVector3D(0.1f, 0.1f, 0.1f));
    m_lightingProgram->setUniformValue("directLight.diffuse", QVector3D(0.8f, 0.8f, 0.8f));
    m_lightingProgram->setUniformValue("directLight.specular", QVector3D(1.0f, 1.0f, 1.0f));
    m_lightingProgram->setUniformValue("directLight.direction", QVector3D(-1.0f, -2.0f, -1.0f));

    m_lightProgram->bind();
    m_lightProgram->setAttributeBuffer("aPos", GL_FLOAT, 0, 3, 8 * sizeof(GLfloat));
    m_lightProgram->enableAttributeArray("aPos");

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);

}

void GLWindow::render()
{

    int currentFrame = (int)(QDateTime::currentMSecsSinceEpoch());
    //qDebug() << currentFrame;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;


    glViewport(0, 0, width(), height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_view.setToIdentity();
    m_view.lookAt(cameraPos, cameraPos + cameraFront, cameraUp );

    m_project.setToIdentity();
    m_project.perspective(fov, 900.f/600.f, 0.1f, 100.0f);

    m_model.setToIdentity();
    m_lightingProgram->bind();
    m_lightingProgram->setUniformValue("projection", m_project);
    m_lightingProgram->setUniformValue("view", m_view);
    m_lightingProgram->setUniformValue("viewPos", cameraPos);
    m_lightingProgram->setUniformValue("spotLight.position", cameraPos);
    m_lightingProgram->setUniformValue("spotLight.direction", cameraFront);

    m_vbo_cube->bind();

    for(unsigned int i = 0; i < 10; i++)
    {
        m_model.translate(cubePositions[i]);
        float angle = 20.0f * i;
        m_model.rotate(angle,QVector3D(1.0f, 0.3f, 0.5f));
        m_lightingProgram->setUniformValue("model", m_model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }



    m_lightProgram->bind();
    m_vbo_cube->bind();
    m_lightProgram->setUniformValue("project", m_project);
    m_lightProgram->setUniformValue("view", m_view);
    for(unsigned int i = 0; i < 4; i++ )
    {
        m_model.setToIdentity();
        m_model.translate(pointLightPositions[i]);
        m_model.scale(0.2f);
        m_lightProgram->setUniformValue("model", m_model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

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
    //qDebug() << keyEvent->text() <<  ", " << keyEvent->key();
    float cameraSpeed = static_cast<float>(0.025 * deltaTime);
    //qDebug() << cameraSpeed;

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

    float sensitivity = 0.07f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

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

void GLWindow::resizeEvent(QResizeEvent *x)
{
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

