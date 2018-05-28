#ifndef SPHERE_H
#define SPHERE_H

#include <GL/glew.h>

class Sphere
{
public:
    Sphere();
    ~Sphere();
    void init();
    void cleanup();
    void update(glm::mat4& pojection, glm::mat4& view, glm::vec3& light);
    void draw();

private:
    bool loadSphere(float r, std::vector < glm::vec3 > & vertices, std::vector < glm::vec2 > & uvs);
//
    int lats;
    int longs;
//
    glm::mat4 m_modelMatrix;
    bool m_isInited;
    GLuint m_vao, m_vboVertex, m_vboIndex;
    GLuint m_uvBuffer;
    GLuint m_programID, m_texture, m_textureID;
    int m_numToDraw;
    //private program uniform ID to use
    GLuint m_vertexPositionID;
    GLuint m_mvpMatrixID;
   
};

#endif // SPHERE_H
