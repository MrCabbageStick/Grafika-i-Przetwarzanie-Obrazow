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

struct Planet{
    float sun_rotation;
    float sun_distance;
    float self_rotation;
    float tilt;
    float scale;

    glm::mat4 getMatrix(glm::mat4 origin, int frame){
        glm::mat4 scale_vec = glm::scale(identity, glm::vec3(scale));

        return getOffset(origin, frame) *  scale_vec;
    }

    glm::mat4 getOffset(glm::mat4 origin, int frame){
        glm::mat4 tx = glm::translate(identity, glm::vec3(sun_distance, 0, 0));
        glm::mat4 rot = glm::rotate(identity, glm::radians(frame * sun_rotation), Z_AXIS);
        glm::mat4 rot_self = glm::rotate(identity, glm::radians(frame * self_rotation), Z_AXIS);
        rot_self = glm::rotate(rot_self, glm::radians(tilt), Y_AXIS);

        return origin * rot * tx * rot_self;
    }
};

void GLWidget::paintGL()
{
    glClearColor(0, 0, 0.1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    Planet earth{
        .sun_rotation = 1.0f,
        .sun_distance = 0.25f,
        .self_rotation = 1.5f,
        .tilt = 0.23f,
        .scale = 0.5f,
    };

    Planet moon{
        .sun_rotation = 10.0f,
        .sun_distance = 0.15f,
        .self_rotation = -10.0f,
        .tilt = 0,
        .scale = 0.15f,
    };

    Planet mars{
        .sun_rotation = 0.6f,
        .sun_distance = 0.75f,
        .self_rotation = 1.25f,
        .tilt = 0,
        .scale = 0.4f,
    };

    Planet phobos{
        .sun_rotation = 7.0f,
        .sun_distance = 0.15f,
        .self_rotation = -7.0f,
        .tilt = 0,
        .scale = 0.15f,
    };

    Planet deimos{
        .sun_rotation = 5.0f,
        .sun_distance = 0.20f,
        .self_rotation = -5.0f,
        .tilt = 0,
        .scale = 0.1f,
    };

    // glm::mat4 view = glm::translate(identity, glm::vec3(0, 0, -4));
    // view = glm::rotate(view, 0.01f * pos_x, Y_AXIS);
    // view = glm::rotate(view, 0.01f * pos_y, X_AXIS);
    // view = glm::scale(view, glm::vec3(zoom));
    // camera.pos.z = -7;
    processCamera();
    glm::mat4 view = camera.matrix();

    glm::mat4 earth_offset = earth.getOffset(identity, frame);
    glm::mat4 mat = earth.getMatrix(identity, frame);
    glm::mat4 mat_moon = moon.getMatrix(earth_offset, frame);

    glm::mat4 mars_offset = mars.getOffset(identity, frame);
    glm::mat4 mat_mars = mars.getMatrix(identity, frame);
    glm::mat4 mat_phobos = phobos.getMatrix(mars_offset, frame);
    glm::mat4 mat_deimos = deimos.getMatrix(mars_offset, frame);

    if(shaders.contains("basic")) {
        shaders["basic"]->use();
        shaders["basic"]->setUniform("ViewMat", view);
        shaders["basic"]->setUniform("ProjectionMat", ProjMat);

        // SUN
        // shaders["basic"]->setUniform("ModelMat", glm::scale(identity, glm::vec3(0.75f)));
        // geometry["main_axes"]->render();
        // shaders["basic"]->setUniform("Color", glm::vec3(1, 1, 0));
        // geometry["box"]->render();

        // shaders["basic"]->setUniform("ModelMat", mat);
        // shaders["basic"]->setUniform("Color", glm::vec3(0, 0.4, 1));
        // geometry["box"]->render();

        // shaders["basic"]->setUniform("ModelMat", mat_moon);
        // shaders["basic"]->setUniform("Color", glm::vec3(0.8, 0.8, 0.9));
        // geometry["box"]->render();

        // shaders["basic"]->setUniform("ModelMat", mat_mars);
        // shaders["basic"]->setUniform("Color", glm::vec3(1, 0.3, 0.0));
        // geometry["box"]->render();

        // shaders["basic"]->setUniform("ModelMat", mat_phobos);
        // shaders["basic"]->setUniform("Color", glm::vec3(0.9, 0.8, 0.8));
        // geometry["box"]->render();

        // shaders["basic"]->setUniform("ModelMat", mat_deimos);
        // shaders["basic"]->setUniform("Color", glm::vec3(0.9, 0.9, 0.8));
        // geometry["box"]->render();

        Frame box_frame;
        box_frame.pos.x += 0.2f;
        auto s = box_frame.s();
        box_frame.forward = glm::vec3((glm::vec4(box_frame.forward, 1.0f) * glm::rotate(identity, frame * 0.01f, s)));
        box_frame.up = glm::cross(glm::normalize(box_frame.forward), s);

        shaders["basic"]->setUniform("ModelMat", box_frame.matrix());
        shaders["basic"]->setUniform("Color", glm::vec3(0.9, 0.9, 0.8));
        geometry["box"]->render();
    }
    else {
        qDebug("WARNING: No shader program");
    }

    frame++;
}
