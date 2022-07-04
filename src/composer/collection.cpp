// SPDX-License-Identifier: GPL-3.0-only

#include <vector>
#include <sstream>
#include <filesystem>
#include <composer/collection.hpp>

namespace Composer {
    template<typename T>
    struct VariableLenghtChunk {
        std::uint32_t lenght;
        T data[];

        std::size_t size() const {
            return sizeof(std::uint32_t) + this->lenght * sizeof(T);
        }
    };

    std::vector<char> merge_pixel_shaders(ps_collection_data_t const &shaders_data) {
        std::vector<char> output;
        
        #define INSERT_CHUNK_LENGHT(lenght_in_bytes, unit) { \
            std::uint32_t lenght = lenght_in_bytes / unit; \
            auto *lenght_bytes = reinterpret_cast<char *>(&lenght); \
            output.insert(output.end(), lenght_bytes, lenght_bytes + sizeof(lenght)); \
        }

        #define INSERT_CHUNK_DATA(data) { \
            output.insert(output.end(), data.begin(), data.end()); \
        }

        // Shader count
        INSERT_CHUNK_LENGHT(shaders_data.size(), 1);

        for(auto &shader_data : shaders_data) {
            // Shader name
            INSERT_CHUNK_LENGHT(shader_data.name.size(), 1);
            INSERT_CHUNK_DATA(shader_data.name);

            // Function count
            INSERT_CHUNK_LENGHT(shader_data.functions.size(), 1);

            for(auto &function_data : shader_data.functions) {
                // Function name
                INSERT_CHUNK_LENGHT(function_data.name.size(), 1);
                INSERT_CHUNK_DATA(function_data.name);

                // Bytecode
                INSERT_CHUNK_LENGHT(function_data.data.size(), sizeof(std::uint32_t));
                INSERT_CHUNK_DATA(function_data.data);
            }
        }

        #undef INSERT_CHUNK_DATA
        #undef INSERT_CHUNK_LENGHT

        return std::move(output);
    }

    ps_collection_data_t explode_pixel_shader_collection(std::vector<char> const &shader_collection) {
        ps_collection_data_t output;
        auto *data = shader_collection.data();
        std::size_t offset = 0;

        #define READ_CHUNK(type, buffer) { \
            auto *chunk = reinterpret_cast<VariableLenghtChunk<type> const *>(data + offset); \
            auto *chunk_data_begin = reinterpret_cast<char const *>(chunk->data); \
            auto *chunk_data_end = reinterpret_cast<char const *>(chunk->data + chunk->lenght); \
            buffer.insert(buffer.begin(), chunk_data_begin, chunk_data_end); \
            offset += chunk->size(); \
        }

        auto shader_count = *reinterpret_cast<const std::uint32_t *>(data);
        offset += sizeof(std::uint32_t);

        for(std::size_t i = 0; i < shader_count; i++) {
            auto &shader = output.emplace_back();

            READ_CHUNK(char, shader.name);

            auto shader_functions_count = *reinterpret_cast<std::uint32_t const *>(data + offset);
            offset += sizeof(std::uint32_t);

            for(std::size_t j = 0; j < shader_functions_count; j++) {
                auto &shader_function = shader.functions.emplace_back();
                READ_CHUNK(char, shader_function.name);
                READ_CHUNK(std::uint32_t, shader_function.data);
            }
        }

        #undef READ_CHUNK

        return std::move(output);
    }

    std::vector<char> merge_vertex_shaders(vs_collection_data_t const &shaders_data) {
        std::vector<char> output;

        for(auto &shader_data : shaders_data) {
            // Insert shader size bytes
            std::uint32_t size = shader_data.size();
            auto *size_bytes = reinterpret_cast<char *>(&size);
            output.insert(output.end(), size_bytes, size_bytes + sizeof(size));

            // Insert shader data
            output.insert(output.end(), shader_data.begin(), shader_data.end());
        }

        return std::move(output);
    }

    vs_collection_data_t explode_vertex_shader_collection(std::vector<char> const &shader_collection) {
        std::vector<std::vector<char>> output;
        auto *data = shader_collection.data();

        std::size_t offset = 0;
        while(offset < shader_collection.size()) {
            auto &shader = output.emplace_back();
            auto *chunk = reinterpret_cast<VariableLenghtChunk<char> const *>(data + offset);
            auto *chunk_data_begin = reinterpret_cast<char const *>(chunk->data);
            auto *chunk_data_end = reinterpret_cast<char const *>(chunk->data + chunk->lenght);
            shader.insert(shader.begin(), chunk_data_begin, chunk_data_end);
            offset += chunk->size(); \
        }

        return std::move(output);
    }
}
