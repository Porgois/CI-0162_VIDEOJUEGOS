#ifndef LAYER_COMPONENT_HPP
#define LAYER_COMPONENT_HPP

struct LayerComponent {
    int z_index;

    LayerComponent(int z_index = 0) : z_index(z_index) {}
};

#endif // LAYER_COMPONENT_HPP