// SPDX-License-Identifier: GPL-3.0-only

#ifndef COMPOSER__FILE_HPP
#define COMPOSER__FILE_HPP

#include <string>
#include <filesystem>
#include <variant>
#include <d3dcommon.h>
#include <composer/collection.hpp>

namespace Composer {
    class ShaderSourceInclude : public ID3DXInclude {
    public:
        HRESULT Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
        HRESULT Close(LPCVOID pData);
        ShaderSourceInclude(std::filesystem::path local, std::filesystem::path system) : m_local_directory(local), m_system_directory(system) {}
        
    private:
        std::filesystem::path m_local_directory;
        std::filesystem::path m_system_directory;
    };

    /**
     * Compile shader from source file
     * @param input_file    shader source file
     * @param entry_point   name of the function where execution begins
     * @param model         shader model; instruction set that will be used
     * @param output_file   shader output file
     * @exception           if compilation process fails
     */
    void compile_shader_file(std::filesystem::path input_file, std::string entry_point, std::string model, std::filesystem::path output_file, std::filesystem::path include_path);

    /**
     * Assemble shader from assembly source file
     * @param input_file        shader assembly source file
     * @param output_file       shader output file
     * @exception               if assemble process fails
     */
    void assemble_shader_file(std::filesystem::path input_file, std::filesystem::path output_file);

    /**
     * Disassemble compiled shader file
     * @param input_file        shader file
     * @param output_file       disassembled shader output file
     * @exception               if disassemble process fails
     */
    void disassemble_shader_file(std::filesystem::path input_file, std::filesystem::path output_file);

    /**
     * Read multiple pixel shaders in a directory
     * @param directory     shaders folder directory
     * @return              pixel shader collection data
     */
    ps_collection_data_t read_pixel_shader_collection_from_files(std::filesystem::path directory);

    /**
     * Merge multiple pixel shader files into a pixel shader collection
     * @param directory     shaders folder directory or shader collection data
     * @param output_file   shader collection output file
     */
    void create_pixel_shader_collection_file(std::variant<std::filesystem::path, ps_collection_data_t> input, std::filesystem::path output_file);

    /**
     * Split a pixel shader collection file into individual shader files
     * @param input                 pixel shader collection input; file path or data
     * @param output_directory      shader files output directory
     */
    void split_pixel_shader_collection_file(std::variant<std::filesystem::path, std::vector<char>> input, std::filesystem::path output_directory);

    /**
     * Read a vertex shader collection file
     * @param file  path to shader collection file
     * @return      shaders data
     */
    vs_collection_data_t read_vertex_shader_collection_from_files(std::filesystem::path file);

    /**
     * Merge multiple vertex shader files into a vertex shader collection 
     * @param input         shaders folder directory or shader collection data
     * @param output_file   shader collection output file
     */
    void create_vertex_shader_collection_file(std::variant<std::filesystem::path, vs_collection_data_t> input, std::filesystem::path output_file);

    /**
     * Split a vertex shader collection into individual shader files
     * @param input                 vertex shader collection inpot; file path or data
     * @param output_directory      shader files output directory
     */
    void split_vertex_shader_collection_file(std::variant<std::filesystem::path, std::vector<char>> input, std::filesystem::path output_directory);

    /**
     * Decrypt Halo's shader file
     * @param input_file    path to encrypted shader file
     * @param output_file   path to output decrypted file
     */
    void decrypt_shader_file(std::filesystem::path input_file, std::filesystem::path output_file);

    /**
     * Encrypt Halo's shader file
     * @param input_file    path to shader file
     * @param output_file   path to output encrypted file
     */
    void encrypt_shader_file(std::filesystem::path input_file, std::filesystem::path output_file);
}

#endif
