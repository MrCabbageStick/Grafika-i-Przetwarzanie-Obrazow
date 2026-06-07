#include "glwidget.h"
#include <QMouseEvent>

#include "glm/glm.hpp"
#include "glm/common.hpp"
#include <glm/ext/matrix_transform.hpp>
#include "primitives.h"

glm::mat4 identity = glm::mat4(1.0f);
constexpr auto Z_AXIS = glm::vec3(0, 0, 1);
constexpr auto X_AXIS = glm::vec3(1, 0, 0);
constexpr auto Y_AXIS = glm::vec3(0, 1, 0);

GLWidget::GLWidget()
{
    frame = 0;
    pos_x = pos_y = 0;
    zoom = 1.0f;
}

GLWidget::~GLWidget()
{
    //TODO: usuwanie shaderow, geometrii, buforow
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    m_logger = new QOpenGLDebugLogger(this);
    if (m_logger->initialize()) {
        connect(m_logger, &QOpenGLDebugLogger::messageLogged, this, [](const QOpenGLDebugMessage &msg) {
            if (msg.id() == 131185 || msg.id() == 131218)
                return;
            qDebug() << msg;
        });
        m_logger->startLogging();
    }

    createShaders();
    createGeometry();

    connect(&frame_timer, &QTimer::timeout, this, QOverload<>::of(&GLWidget::update));
    frame_timer.setInterval(10);
    frame_timer.start();


}


void GLWidget::createShaders()
{
    shaders["basic"] = new GLSLProgram;
    bool stat = shaders["basic"]->compileShaderFromFile("shaders/vs.glsl", GL_VERTEX_SHADER);
    stat &= shaders["basic"]->compileShaderFromFile("shaders/fs.glsl", GL_FRAGMENT_SHADER);
    stat &= shaders["basic"]->link();
    if (!stat) qFatal("Some problem with shader!");

    shaders["ads"] = new GLSLProgram;
    stat = shaders["ads"]->compileShaderFromFile("shaders/ads_vs.glsl", GL_VERTEX_SHADER);
    stat &= shaders["ads"]->compileShaderFromFile("shaders/ads_fs.glsl", GL_FRAGMENT_SHADER);
    stat &= shaders["ads"]->link();
    if (!stat) qFatal("Some problem with shader!");
}

void GLWidget::createGeometry()
{
    geometry["main_axes"] = newAxesGeometry();
    geometryMat["main_axes"] = glm::mat4(1);

    geometry["plane"] = newPlaneGeometry({0.1,0.1}, {1, 1, 1});
    geometryMat["plane"] = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));

    geometry["box"] = newBoxGeometry({0.1,0.1,0.1}, {1, 1, 1});
    geometryMat["box"] = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));

    geometry["circ"] = newCircleGeometry(0.75, 24, {1,0,0});
    geometryMat["circ"] = glm::mat4(1.0f);

    geometry["cone"] = newConeGeometry(0.5, 0.75, 20, {1, 1, 1});
    geometryMat["cone"] = glm::mat4(1.0f);
}

void GLWidget::mousePressEvent(QMouseEvent *e){
    if(e->button() == Qt::LeftButton){
        pos_x = e->pos().x();
        pos_y = e->pos().y();
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    move_x += e->pos().x() - pos_x;
    move_y += e->pos().y() - pos_y;

    pos_x = e->pos().x();
    pos_y = e->pos().y();
}

void GLWidget::wheelEvent(QWheelEvent *e)
{
    zoom += e->angleDelta().y()/10000.0f;
    zoom = glm::clamp(zoom, 0.1f, 10.0f);
}

void GLWidget::keyPressEvent(QKeyEvent *e){
    keys.insert(e->key());
}

void GLWidget::keyReleaseEvent(QKeyEvent *e){
    keys.remove(e->key());
}

void GLWidget::processCamera(){
    constexpr float MOVEMENT_MULT = 0.05;
    constexpr float ROTATION_MULT = 0.01;

    if(keys.contains(Qt::Key_W)){
        camera.pos -= camera.forward * MOVEMENT_MULT;
    }
    if(keys.contains(Qt::Key_S)){
        camera.pos += camera.forward * MOVEMENT_MULT;
    }
    if(keys.contains(Qt::Key_A)){
        camera.pos -= camera.s() * MOVEMENT_MULT;
    }
    if(keys.contains(Qt::Key_D)){
        camera.pos += camera.s() * MOVEMENT_MULT;
    }
    if(keys.contains(Qt::Key_Q)){
        camera.pos += camera.up * MOVEMENT_MULT;
    }
    if(keys.contains(Qt::Key_Z)){
        camera.pos -= camera.up * MOVEMENT_MULT;
    }

    glm::mat4 rot = glm::rotate(identity, ROTATION_MULT * move_x, Y_AXIS);
    rot = glm::rotate(rot, ROTATION_MULT * move_y, camera.s());
    camera.forward = glm::normalize(glm::vec3(rot * glm::vec4(camera.forward, 0.0f)));
    camera.up = glm::normalize(glm::vec3(rot * glm::vec4(camera.up, 0.0f)));

    move_x = 0;
    move_y = 0;
}

#include <glm/gtc/matrix_transform.hpp>

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    float aspect_ratio = w / (float)h;
    // glm::ortho()
    ProjMat = glm::perspective(glm::radians(60.0f), aspect_ratio, 0.1f, 10.0f);
}

void GLWidget::paintGL()
{
    glClearColor(0, 0, 0.1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    processCamera();
    glm::mat4 view = camera.matrix();

    if(shaders.contains("basic")) {
        Frame box_frame;
        box_frame.pos = {2, 0, 0};
        Frame cone_frame;
        cone_frame.pos = {0, 0, 2};

        shaders["basic"]->use();
        shaders["basic"]->setUniform("ViewMat", view);
        shaders["basic"]->setUniform("ProjectionMat", ProjMat);

        auto box_scale = glm::scale(identity, {1.5, 1.5, 1.5});
        shaders["basic"]->setUniform("ModelMat", box_frame.matrix() * box_scale);
        shaders["basic"]->setUniform("Color", glm::vec3(0.9, 0.9, 0.8));
        geometry["box"]->render();

        auto cone_scale = glm::scale(identity, {1.5, 1.5, 1.5});
        shaders["basic"]->setUniform("ModelMat", cone_frame.matrix() * cone_scale);
        shaders["basic"]->setUniform("Color", glm::vec3(0.9, 0.9, 0.8));
        geometry["cone"]->render();

    }
    else {
        qDebug("WARNING: No basic shader program");
    }

    if(shaders.contains("ads")){
        Frame box_frame;
        box_frame.pos = {-2, 0, 0};
        Frame cone_frame;
        cone_frame.pos = {0, 0, -2};

        shaders["ads"]->use();
        shaders["ads"]->setUniform("ViewMat", view);
        shaders["ads"]->setUniform("ProjectionMat", ProjMat);


        shaders["ads"]->setUniform("LightPos",     glm::vec3(2.0f, 3.0f, 2.0f));
        shaders["ads"]->setUniform("LightColor",   glm::vec3(1.0f, 1.0f, 1.0f));
        shaders["ads"]->setUniform("AmbientColor", glm::vec3(0.1f, 0.1f, 0.1f));
        shaders["ads"]->setUniform("Shininess",    32.0f);

        auto box_scale = glm::scale(identity, {1.5, 1.5, 1.5});
        shaders["ads"]->setUniform("ModelMat", box_frame.matrix() * box_scale);
        shaders["ads"]->setUniform("Color", glm::vec3(0.9, 0.9, 0.8));
        geometry["box"]->render();


        shaders["ads"]->setUniform("LightPos",     glm::vec3(2.0f, 3.0f, 2.0f));
        shaders["ads"]->setUniform("LightColor",   glm::vec3(1.0f, 1.0f, 1.0f));
        shaders["ads"]->setUniform("AmbientColor", glm::vec3(0.1f, 0.1f, 0.1f));
        shaders["ads"]->setUniform("Shininess",    32.0f);

        auto cone_scale = glm::scale(identity, {1.5, 1.5, 1.5});
        shaders["ads"]->setUniform("ModelMat", cone_frame.matrix() * cone_scale);
        shaders["ads"]->setUniform("Color", glm::vec3(0.9, 0.9, 0.8));
        geometry["cone"]->render();
    }
    else {
        qDebug("WARNING: No ads shader program");
    }


    frame++;
}
