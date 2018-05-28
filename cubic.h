#ifndef Cubic_H
#define Cubic_H

#include <GL/glew.h>

class Cubic
{
public:
    Cubic();
    ~Cubic();
    void init();
    void cleanup();
    void update(glm::mat4& pojection, glm::mat4& view, glm::vec3& light);
    void draw();

private:

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

#endif // Cubic_H
