#ifndef MESH_H
#define MESH_H

#include <QObject>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include <QVector3D>
#include <QVector2D>

#include <string>
#include <vector>

using namespace std;

const int MAX_BONE_INFLUENCE = 4;

struct Vertex {
    QVector3D Position;
    QVector3D Normal;
    QVector2D TexCoords;
    QVector3D Tangent;
    QVector3D Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};
struct Texture{
    GLuint id;
    string type;
    string path;
};

class Mesh : public QObject
{
    Q_OBJECT
public:
    Mesh(const vector<Vertex>& vertices,
         const vector<unsigned int>& indices,
         const vector<Texture>& textures,
         QObject* parent = nullptr);

    Mesh(const Mesh& mesh, QObject* parent = nullptr);
public:
    // mesh data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;

    void Draw(QOpenGLShaderProgram &shader);
private:
    //  render data
    unsigned int VAO, VBO, EBO;
    QOpenGLVertexArrayObject* m_vao;
    QOpenGLBuffer* m_vbo;
    QOpenGLBuffer* m_ebo;
private:
    QOpenGLFunctions_3_3_Core* m_func3_3_core;
private:
    void setupMesh();
signals:
};

#endif // MESH_H
