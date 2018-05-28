#ifndef CAR_H
#define CAR_H

#include <GL/glew.h>

class Car : BasicObject
{
public:
    Car();
    ~Car();
    void init();
    void cleanup();
    void update(glm::mat4& pojection, glm::mat4& view, glm::vec3& light);
    void transform(mat4& transform);   
    void draw();

protected:
//    glm::mat4 m_modelMatrix;
//  bool m_isInited;
//  GLuint m_vao, m_vboVertex, m_vboIndex;
//  GLuint m_uvBuffer, m_normalBuffer;
//  GLuint m_programID, m_texture, m_textureID, m_lightID;
// int m_numToDraw;
//program uniform ID to use
//    GLuint m_mvpMatrixID;
//    GLuint m_viewMatrixID;
//    GLuint m_modelMatrixID;
};


#endif // CAR_H
