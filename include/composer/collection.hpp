// SPDX-License-Identifier: GPL-3.0-only

#ifndef COMPOSER__COLLECTION_HPP
#define COMPOSER__COLLECTION_HPP

#include <string>
#include <vector>

namespace Composer {
    struct PSCollectionShader {
        struct Function {
            std::string name;
            std::vector<char> data;
        };

        std::string name;
        std::vector<Function> functions;

        PSCollectionShader() = default;
    };

    using ps_collection_data_t = std::vector<PSCollectionShader>;
    using vs_collection_data_t = std::vector<std::vector<char>>;

    /**
     * Merge multiple pixel shader into a pixel shader collection
     * @param shaders_data              data array to be merged
     * @return                          pixel shader collection
     */
    std::vector<char> merge_pixel_shaders(ps_collection_data_t const &shaders_data);

    /**
     * Split a pixel shader collection into individual shaders
     * @param shader_collection     shader collection data
     * @return                      pixel shaders data 
     */
    ps_collection_data_t explode_pixel_shader_collection(std::vector<char> const &shader_collection);

    /**
     * Merge multiple vertex shader into a vertex shader collection
     * @param shaders_data              data to be merged
     * @return                          vertex shader collection
     */
    std::vector<char> merge_vertex_shaders(vs_collection_data_t const &shaders_data);

    /**
     * Split a vertex shader collection into individual shaders
     * @param shader_collection     shader collection data
     * @return                      pixel shaders data 
     */
    vs_collection_data_t explode_vertex_shader_collection(std::vector<char> const &shader_collection);
}

#endif
