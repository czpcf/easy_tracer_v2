#include "resource.hpp"

Resource::Resource() {
}

ResourceGroup::ResourceGroup() {
}

Geometry *ResourceGroup::get_shape(int n) {
    return get_info(n)->get_shape();
}