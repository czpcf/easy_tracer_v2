/*
resource
*/
#ifndef EMITTOR_H
#define EMITTOR_H

#include <vector>
#include <algorithm>
#include "resource.hpp"
#include "../tensor/vec2.hpp"
#include "../tensor/vec3.hpp"
#include "../light/light.hpp"

class ResourceLight: public Resource {
public:
    ResourceLight();
    virtual ~ResourceLight() = default;

    /// @brief return irradiance
    virtual Vec3 get_emittor() = 0;

    /// @brief return the light
    virtual Light *get_light() = 0;
};

class ResourceGroupLight: public ResourceGroup {
public:
    ResourceGroupLight();
    virtual ~ResourceGroupLight() = default;

    /// @brief return the n-th light
    virtual Light *get_light(int n) = 0;

    /// @brief return the resource of the n-th shape
    virtual ResourceLight* get_info(int n) override = 0;
};
#endif
