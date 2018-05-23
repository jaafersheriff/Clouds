#pragma once
#ifndef _CONE_TRACE_SHADER_HPP_
#define _CONE_TRACE_SHADER_HPP_

#include "Shader.hpp"
#include "Volume.hpp"

class ConeTraceShader : public Shader {
    public:
        ConeTraceShader(const std::string &r, const std::string &v, const std::string &f) :
            Shader(r, v, f)
        {}

        void coneTrace(Volume *);

        /* Cone trace parameters */
        int vctSteps = 16;
        float vctConeAngle = 0.784398163f;
        float vctConeInitialHeight = 0.1f;
        float vctLodOffset = 0.1f;

    private:
        void bindVolume(Volume *);
        void unbindVolume();
};

#endif