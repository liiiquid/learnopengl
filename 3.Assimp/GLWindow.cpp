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

//! [2]
void GLWindow::render(QPainter *painter)
{
    Q_UNUSED(painter);
}
void GLWindow::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/glsl/survivalbackpack.vsh");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/glsl/survivalbackpack.fsh");
    m_program->link();
    string directory = "C:/Users/prior/Desktop/opengl/qt/3.Assimp/model3d/";
    string directory_slash = "C:\\Users\\prior\\Desktop\\opengl\\qt\\3.Assimp\\model3d\\";
    //m_model1 = new Model("C:/Users/prior/Desktop/opengl/qt/3.Assimp/model3d/Survival_BackPack_2/Survival_BackPack_2.fbx", false);
    //m_model1 = new Model("C:/Users/prior/Desktop/opengl/qt/3.Assimp/model3d/cabinet/柜子fbx.fbx", false);
    //m_model1 = new Model("C:/Users/prior/Desktop/opengl/qt/3.Assimp/model3d/cabinet/单柜(1).obj", false);
    //m_model1 = new Model("C:/Users/prior/Desktop/opengl/qt/3.Assimp/model3d/Simple BunkBed/Bunk Bed.obj", false);
    m_model1 = new Model(directory_slash + "柜子\\" + "柜子.obj", false); // 法线数据丢失
    qDebug() << m_model1->directory.c_str();

    glClearColor(0.7f, 0.7f, 0.7f, 0.7f);
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
    m_project.perspective(fov, 900.f/600.f, 0.1f, 1000.0f);

    m_model.setToIdentity();
    m_model.scale(0.01f);
    m_program->bind();
    m_program->setUniformValue("view", m_view);
    m_program->setUniformValue("model", m_model);
    m_program->setUniformValue("projection", m_project);
    m_program->setUniformValue("directLight.ambient", QVector3D(0.3,0.3,0.3));
    m_program->setUniformValue("directLight.diffuse", QVector3D(0.5,0.5,0.5));
    m_program->setUniformValue("directLight.specular", QVector3D(0.2, 0.2, 0.2));
    m_program->setUniformValue("directLight.direction", QVector3D(-1.0, -2.0, -1.0));
    m_program->setUniformValue("viewPos", cameraPos);
    m_model1->Draw(*m_program);

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

   if(keyEvent->text() == 'w')
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
    else if(keyEvent->text() == '4')
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

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    firstPerson(xoffset, yoffset);

}

void GLWindow::resizeEvent(QResizeEvent *x)
{
}

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

