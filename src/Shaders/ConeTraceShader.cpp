#include "ConeTraceShader.hpp"

#include "Camera.hpp"
#include "Light.hpp"
#include "Library.hpp"

ConeTraceShader::ConeTraceShader(std::string v, std::string f) :
    Shader(v, f) {
    init();
}

void ConeTraceShader::coneTrace(Cloud *cloud) {
    bind();
    for (auto volume : cloud->volumes) {
        bindVolume(volume);

        /* Bind cone tracing params */
        loadInt(getUniform("vctSteps"), vctSteps);
        loadFloat(getUniform("vctConeAngle"), vctConeAngle);
        loadFloat(getUniform("vctConeInitialHeight"), vctConeInitialHeight);
        loadFloat(getUniform("vctLodOffset"), vctLodOffset);

        /* Cone trace from the camera's perspective */
        loadVector(getUniform("center"), cloud->position + volume->quadOffset);
        loadFloat(getUniform("scale"), volume->quadScale.x);
        loadVector(getUniform("lightPos"), Light::spatial.position);

        /* Bind quad */
        /* VAO */
        CHECK_GL_CALL(glBindVertexArray(Library::quad->vaoId));

        /* Vertices and normals VBO */
        // TODO : unnecessary - clean up
        int pos = getAttribute("vertPos");
        CHECK_GL_CALL(glEnableVertexAttribArray(pos));
        CHECK_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, Library::quad->vertBufId));
        CHECK_GL_CALL(glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, nullptr));
        pos = getAttribute("vertNor");
        CHECK_GL_CALL(glEnableVertexAttribArray(pos));
        CHECK_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, Library::quad->norBufId));
        CHECK_GL_CALL(glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, nullptr));


        loadMatrix(getUniform("P"), &Camera::getP());
        loadMatrix(getUniform("V"), &Camera::getV());
        glm::mat4 Vi = Camera::getV();
        Vi[3][0] = Vi[3][1] = Vi[3][2] = 0.f;
        Vi = glm::transpose(Vi);
        loadMatrix(getUniform("Vi"), &Vi);

        glm::mat4 M = glm::translate(glm::mat4(1.f), cloud->position + volume->quadOffset);
        M *= glm::scale(glm::mat4(1.f), glm::vec3(volume->quadScale.x));
        loadMatrix(getUniform("M"), &M);

        glm::mat3 N = glm::mat3(transpose(inverse(M * Vi)));
        loadMatrix(getUniform("N"), &N);
        CHECK_GL_CALL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

        /* Wrap up shader */
        CHECK_GL_CALL(glBindVertexArray(0));

        unbindVolume();
    }
    unbind();
}

void ConeTraceShader::bindVolume(Volume *volume) {
    CHECK_GL_CALL(glActiveTexture(GL_TEXTURE0 + volume->volId));
    CHECK_GL_CALL(glBindTexture(GL_TEXTURE_3D, volume->volId));
    loadInt(getUniform("volumeTexture"), volume->volId);
    
    loadInt(getUniform("voxelDim"), volume->dimension);
    loadVector(getUniform("xBounds"), volume->xBounds);
    loadVector(getUniform("yBounds"), volume->yBounds);
    loadVector(getUniform("zBounds"), volume->zBounds);
}

void ConeTraceShader::unbindVolume() {
    CHECK_GL_CALL(glActiveTexture(GL_TEXTURE0));
    CHECK_GL_CALL(glBindTexture(GL_TEXTURE_3D, 0));
}
