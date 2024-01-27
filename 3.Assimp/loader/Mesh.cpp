#include "Mesh.h"

Mesh::Mesh(const vector<Vertex> &vertices,
           const vector<unsigned int> &indices,
           const vector<Texture> &textures,
           QObject* parent)
    : QObject{parent}
{
    m_func3_3_core = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

Mesh::Mesh(const Mesh &mesh, QObject *parent)
    : QObject(parent)
{
    m_func3_3_core = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    this->vertices = mesh.vertices;
    this->indices = mesh.indices;
    this->textures = mesh.textures;
    setupMesh();
}
void Mesh::Draw(QOpenGLShaderProgram &shader)
{
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;

    for( int i = 0; i < textures.size(); i++)
    {
        m_func3_3_core->glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to string
        else if(name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to string
        else if(name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to string
        // now set the sampler to the correct texture unit
        shader.setUniformValue((name + number).c_str(), i);
        // and finally bind the texture
        m_func3_3_core->glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // draw mesh
    m_vao->bind();
    //m_func3_3_core->glBindVertexArray(VAO);
    //m_func3_3_core->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    m_func3_3_core->glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    m_func3_3_core->glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    m_func3_3_core->glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
    m_vao = new QOpenGLVertexArrayObject();
    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_ebo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_vao->create();
    m_vbo->create();
    m_ebo->create();

    m_vao->bind();
    m_vbo->bind();
    m_ebo->bind();

    m_vbo->setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_ebo->setUsagePattern( QOpenGLBuffer::StaticDraw );

    m_vbo->allocate(&vertices[0], vertices.size() * sizeof(Vertex));
    m_ebo->allocate(&indices[0], indices.size() * sizeof(unsigned int));

    m_func3_3_core->glEnableVertexAttribArray(0);
    m_func3_3_core->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    m_func3_3_core->glEnableVertexAttribArray(1);
    m_func3_3_core->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    m_func3_3_core->glEnableVertexAttribArray(2);
    m_func3_3_core->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    m_func3_3_core->glEnableVertexAttribArray(3);
    m_func3_3_core->glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    m_func3_3_core->glEnableVertexAttribArray(4);
    m_func3_3_core->glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    // ids
    m_func3_3_core->glEnableVertexAttribArray(5);
    m_func3_3_core->glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

    // weights
    m_func3_3_core->glEnableVertexAttribArray(6);
    m_func3_3_core->glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    m_func3_3_core->glBindVertexArray(0);

}
