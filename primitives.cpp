#include "primitives.h"

Geometry *newAxesGeometry()
{
    Geometry* geometry = new Geometry();
    {
        geometry->setPrimitiveMode(GL_LINES);
        glm::vec3 verts[] = { {0,0,0}, {1.0,0,0}, {0,0,0}, {0,1.0,0}, {0,0,0}, {0,0,1.0} };
        glm::vec3 colors[] = { {1,0,0}, {1,0,0},  {0,1,0}, {0,1,0},   {0,0,1}, {0,0,1} };
        geometry->setVertices((int)Attributes::position, verts, 6);
        geometry->setAttribute((int)Attributes::color, colors, 6);
    }
    return geometry;
}

Geometry *newPlaneGeometry(glm::vec2 size, glm::vec3 color)
{
    Geometry* geometry = new Geometry();
    {
        geometry->setPrimitiveMode(GL_TRIANGLES);
        glm::vec3 verts[]  = { {-size.x,-size.y,0}, {-size.x,size.y,0}, {size.x,size.y,0}, {size.x,-size.y,0}};
        glm::vec3 colors[] = { color, color, color, color};
        uint indices[] = {0,1,2,2,3,0};
        geometry->setIndices(indices, 6);
        geometry->setVertices((int)Attributes::position, verts, 4);
        geometry->setAttribute((int)Attributes::color, colors, 4);
    }
    return geometry;
}

Geometry* newBoxGeometry(glm::vec3 size, glm::vec3 color)
{
    Geometry* geometry = new Geometry();
    geometry->setPrimitiveMode(GL_TRIANGLES);
    {
        int n = 24;
        glm::vec3 origin = {0.0, 0.0, 0.0};
        float w = size.x;
        float h = size.y;
        float d = size.z;

        std::vector<glm::vec3> pos(n);
        std::vector<glm::vec3> colors(n);
        std::vector<uint> indices(36);

        int it=0;
        // sciana z = +1  //front
        pos[it++] = origin + glm::vec3{-w,  h, d};
        pos[it++] = origin + glm::vec3{-w, -h, d};
        pos[it++] = origin + glm::vec3{ w, -h, d};
        pos[it++] = origin + glm::vec3{ w,  h, d};

        // sciana x=+1   //right
        pos[it++] = origin + glm::vec3{w,  h, d};
        pos[it++] = origin + glm::vec3{w, -h, d};
        pos[it++] = origin + glm::vec3{w, -h, -d};
        pos[it++] = origin + glm::vec3{w,  h, -d};

        // sciana z=-1   //back
        pos[it++] = origin + glm::vec3{-w,  h, -d};
        pos[it++] = origin + glm::vec3{-w, -h, -d};
        pos[it++] = origin + glm::vec3{ w, -h, -d};
        pos[it++] = origin + glm::vec3{ w,  h, -d};

        // sciana x=-1   //left
        pos[it++] = origin + glm::vec3{-w,  h, d};
        pos[it++] = origin + glm::vec3{-w, -h, d};
        pos[it++] = origin + glm::vec3{-w, -h, -d};
        pos[it++] = origin + glm::vec3{-w,  h, -d};

        // sciana y=1  //top
        pos[it++] = origin + glm::vec3{-w, h, -d};
        pos[it++] = origin + glm::vec3{-w, h,  d};
        pos[it++] = origin + glm::vec3{ w, h,  d};
        pos[it++] = origin + glm::vec3{ w, h, -d};

        // sciana y=-1  //bottom
        pos[it++] = origin + glm::vec3{-w, -h, -d};
        pos[it++] = origin + glm::vec3{-w, -h,  d};
        pos[it++] = origin + glm::vec3{ w, -h,  d};
        pos[it++] = origin + glm::vec3{ w, -h, -d};

        for(int i=0; i<n; i++)
            colors[i] = color;
        geometry->setVertices((int)Attributes::position, pos.data(), n);
        geometry->setAttribute((int)Attributes::color, colors.data(), n);

        it=0;
        indices[it++] = 0;    indices[it++] = 1;    indices[it++] = 2;    indices[it++] = 0;    indices[it++] = 2;    indices[it++] = 3;
        indices[it++] = 4;    indices[it++] = 5;    indices[it++] = 6;    indices[it++] = 4;    indices[it++] = 6;    indices[it++] = 7;
        indices[it++] = 8;    indices[it++] = 11;   indices[it++] = 10;   indices[it++] = 8;    indices[it++] = 10;   indices[it++] = 9;
        indices[it++] = 12;   indices[it++] = 15;   indices[it++] = 14;   indices[it++] = 12;   indices[it++] = 14;   indices[it++] = 13;
        indices[it++] = 16;   indices[it++] = 17;   indices[it++] = 18;   indices[it++] = 16;   indices[it++] = 18;   indices[it++] = 19;
        indices[it++] = 20;   indices[it++] = 23;   indices[it++] = 22;   indices[it++] = 20;   indices[it++] = 22;   indices[it++] = 21;

        geometry->setIndices(indices.data(), indices.size());
    }
    return geometry;
}

Geometry* newCircleGeometry(float radius, int n_segments, glm::vec3 color)
{
    Geometry* geometry = new Geometry();
    {
        geometry->setPrimitiveMode(GL_TRIANGLES);
        std::vector<glm::vec3> verts;
        std::vector<glm::vec3> colors;
        std::vector<uint> indices;

        verts.push_back({0,0,0});
        colors.push_back(color);
        float a=0.0f;

        for(int i=0; i<n_segments; i++){
            float x = radius * cos(a);
            float y = radius * sin(a);
            verts.push_back({x,y,0});
            colors.push_back(color);
            a += 2*M_PI/n_segments;
        }

        for(int i=0; i<n_segments; i++){
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i+1);
        }
        indices.push_back(0);
        indices.push_back(n_segments);
        indices.push_back(1);


        geometry->setIndices(indices.data(), indices.size());
        geometry->setVertices((int)Attributes::position, verts.data(), verts.size());
        geometry->setAttribute((int)Attributes::color, colors.data(), colors.size());
    }
    return geometry;
}
