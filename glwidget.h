#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "glslprogram.h"
#include "geometry.h"

#if QT_VERSION_MAJOR >= 6
    #include <QtOpenGLWidgets/QOpenGLWidget>
#else
    #include <QOpenGLWidget>
#endif

#include <QOpenGLFunctions_4_2_Core>
#include <QOpenGLDebugLogger>
#include <QMap>
#include <QDebug>
#include <QTimer>
#include "glm/glm.hpp"


class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_2_Core
{
    QMap<QString, GLSLProgram*> shaders;
    QMap<QString, Geometry*> geometry;
    QMap<QString, glm::mat4> geometryMat;

    glm::mat4 MVMat;
    glm::mat4 ProjMat;

    QOpenGLDebugLogger *m_logger = nullptr;
    QTimer frame_timer;
    int frame;

    void createShaders();
    void createGeometry();

    int pos_x, pos_y;
    float zoom;

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);


    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
public:
    GLWidget();
    ~GLWidget();
};


#endif // GLWIDGET_H
