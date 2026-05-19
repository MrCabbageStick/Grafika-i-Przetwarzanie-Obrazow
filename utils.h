#ifndef UTILS_H
#define UTILS_H

#include <QDebug>

#include <GL/gl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

enum class Attributes{
    position = 0,
    color = 1,
    normal = 2,
    uv1 = 7,
};

struct Frame{
    glm::vec3 pos;
    glm::vec3 up;
    glm::vec3 forward;

    Frame(){
        pos = glm::vec3(0.0f, 0.0f, 0.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        forward = glm::vec3(0.0f, 0.0f, 1.0f);
    }

    glm::vec3 s() const { return glm::cross(up, forward); }

    glm::mat4 matrix() const {
        glm::mat4 m(1.0f);
        m[0] = glm::vec4(s(), 0);
        m[1] = glm::vec4(up, 0);
        m[2] = glm::vec4(forward, 0);
        m[3] = glm::vec4(pos, 1);

        return m;
    }
};

struct Camera{
    glm::vec3 pos;
    glm::vec3 up;
    glm::vec3 forward;

    Camera(){
        pos = glm::vec3(0.0f, 0.0f, 0.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        forward = glm::vec3(0.0f, 0.0f, -1.0f);
    }

    glm::vec3 s() const { return glm::cross(forward, up); }

    glm::mat4 matrix() const {
        glm::mat4 m(1.0f);

        glm::vec3 xx = s();
        glm::vec3 f = glm::normalize(forward);
        glm::vec3 u = glm::normalize(glm::cross(xx, forward));


        m[0] = glm::vec4(xx, 0);
        m[1] = glm::vec4(u, 0);
        m[2] = glm::vec4(f, 0);
        m = glm::transpose(m);

        glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(-pos.x, -pos.y, -pos.z));

        return m * translate;
    }
};



#endif // UTILS_H
