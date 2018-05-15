#pragma once
#ifndef _LIGHT_HPP_
#define _LIGHT_HPP_

#include "Spatial.hpp"

#include "glm/gtc/matrix_transform.hpp"

class Light {
    public:
        static Spatial spatial;
        static glm::mat4 V;

        static void update(glm::vec3 lookAt) {
            V = glm::lookAt(spatial.position, lookAt, glm::vec3(0, 1, 0));
        }
};

#endif
