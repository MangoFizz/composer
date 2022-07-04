// SPDX-License-Identifier: GPL-3.0-only

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <variant>
#include <stdexcept>
#include <composer/compiler.hpp>
#include <composer/encrypt.hpp>
#include <composer/file.hpp>

namespace Composer {
    static std::vector<char> read_file(std::filesystem::path filepath) {
        if(!std::filesystem::exists(filepath)) {
            std::stringstream error;
            error << "Input file '" << filepath << "' does not exists!";
            throw std::runtime_error(error.str());
        }

        try {
            std::ifstream file;
            file.open(filepath, std::ios_base::in | std::ios_base::binary);
            
            // Get file size
            file.seekg(0, std::ios::end);
            std::size_t filesize = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<char> buffer(filesize);
            file.read(buffer.data(), filesize);

            file.close();

            return std::move(buffer);
        }
        catch(...) {
            throw;
        }
    }

    static void write_file(std::filesystem::path filepath, std::vector<char> data) {
        try {
            std::ofstream file;
            file.open(filepath, std::ios_base::out | std::ios_base::binary);

            file.write(data.data(), data.size());
            file.close();
        }
        catch(...) {
            throw;
        }
    }

    static bool numeric_compare(const std::filesystem::path& a, const std::filesystem::path& b) {
        return (a.string().size() < b.string().size()) || (a.string().size() == b.string().size() && a.string() < b.string());
    }

    HRESULT ShaderSourceInclude::Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes) {
        std::filesystem::path final_path;
        switch(IncludeType) {

            case D3DXINC_LOCAL:
                final_path = m_local_directory / pFileName;
                break;
            
            case D3DXINC_SYSTEM:
                final_path = m_system_directory / pFileName;
                break;
            
            default:
                return E_FAIL;
        }

        try {
            auto read_data = read_file(final_path);

            if(!read_data.empty()) {
                auto *buffer = new char[read_data.size()];
                std::copy(read_data.begin(), read_data.end(), buffer);

                *ppData = buffer;
                *pBytes = read_data.size();
            }
            else {
                *ppData = nullptr;
                *pBytes = 0;
            }
        }
        catch(...) {
            return E_FAIL;
        }

        return S_OK;
    }

    HRESULT ShaderSourceInclude::Close(LPCVOID pData) {
        if(!pData) {
            return E_FAIL;
        }

        auto *buffer = (char *)pData;
        delete[] buffer;
        return S_OK;
    }

    void compile_shader_file(std::filesystem::path input_file, std::string entry_point, std::string shader_profile, std::filesystem::path output_file, std::filesystem::path system_include = {}) {
        std::stringstream error;
        
        std::vector<char> input_data;
        try {
            input_data = read_file(input_file);
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to read input file!" << std::endl;
            throw std::runtime_error(error.str());
        }

        // Source include path
        auto source_include = Composer::ShaderSourceInclude(input_file.parent_path(), system_include);

        std::vector<char> output_data;
        try {
            output_data = compile_shader(input_data, entry_point, shader_profile, true, &source_include);
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to compile shader!" << std::endl;
            throw std::runtime_error(error.str());
        }

        try {
            write_file(output_file, output_data);
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to write output file!" << std::endl;
            throw std::runtime_error(error.str());
        }
    }

    void assemble_shader_file(std::filesystem::path input_file, std::filesystem::path output_file) {
        std::stringstream error;

        std::vector<char> input_data;
        try {
            input_data = read_file(input_file);
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to read input file!" << std::endl;
            throw std::runtime_error(error.str());
        }

        std::vector<char> output_data;
        try {
            output_data = assemble_shader(input_data);
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to assemble shader!" << std::endl;
            throw std::runtime_error(error.str());
        }

        try {
            write_file(output_file, output_data);
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to write output file!" << std::endl;
            throw std::runtime_error(error.str());
        }
    }

    void disassemble_shader_file(std::filesystem::path input_file, std::filesystem::path output_file) {
        std::stringstream error;
        
        std::vector<char> input_data;
        try {
            input_data = read_file(input_file);
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to read input file!" << std::endl;
            throw std::runtime_error(error.str());
        }

        std::vector<char> output_data;
        try {
            output_data = disassemble_shader(input_data);
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to assemble shader!" << std::endl;
            throw std::runtime_error(error.str());
        }

        try {
            write_file(output_file, output_data);
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to write output file!" << std::endl;
            throw std::runtime_error(error.str());
        }
    }

    ps_collection_data_t read_pixel_shader_collection_from_files(std::filesystem::path directory) {
        std::stringstream error;

        if(!std::filesystem::exists(directory)) {
            error << "Directory '" << directory << "' does not exists!" << std::endl;
            throw std::runtime_error(error.str());
        }

        ps_collection_data_t input_data;
        try {
            bool empty_input = true;
            
            for(const auto &entry : std::filesystem::directory_iterator(directory)) {
                auto entry_path = entry.path();
                if(entry.is_directory()) {
                    auto &shader = input_data.emplace_back();
                    shader.name = entry_path.filename().string();

                    std::vector<std::filesystem::path> directory_files;
                    std::copy(std::filesystem::directory_iterator(entry_path), std::filesystem::directory_iterator(), std::back_inserter(directory_files));
                    std::sort(directory_files.begin(), directory_files.end(), numeric_compare);

                    for(auto &file_path : directory_files) {
                        if(std::filesystem::is_regular_file(file_path) && file_path.extension() == ".bin") {
                            empty_input = false;

                            auto &shader_function = shader.functions.emplace_back();
                            auto file_name = file_path.stem().string();

                            // Strip function index from name
                            auto prefix_separator_pos = file_name.find('_');
                            if(prefix_separator_pos != std::string::npos && file_name.size() > prefix_separator_pos + 1) {
                                file_name = file_name.substr(prefix_separator_pos + 1);
                            }

                            shader_function.name = file_name;
                            shader_function.data = read_file(file_path);
                        }
                    }

                }
            }
            
            if(empty_input) {
                throw std::runtime_error("No shaders were found: invalid directory structure for pixel shader collection.");
            }
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to read shader files!" << std::endl;
            throw std::runtime_error(error.str());
        }

        return std::move(input_data);
    }

    void create_pixel_shader_collection_file(std::variant<std::filesystem::path, ps_collection_data_t> input, std::filesystem::path output_file) {
        std::stringstream error;

        ps_collection_data_t input_data;
        if(std::holds_alternative<std::filesystem::path>(input)) {
            try {
                auto directory = std::get<std::filesystem::path>(input);
                input_data = read_pixel_shader_collection_from_files(directory);
            }
            catch(const std::runtime_error e) {
                throw;
            }
        }
        else {
            input_data = std::get<ps_collection_data_t>(input);
        }

        std::vector<char> output_data;
        try {
            output_data = merge_pixel_shaders(input_data);
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to merge pixel shader!" << std::endl;
            throw std::runtime_error(error.str());
        }

        try {
            write_file(output_file, output_data);
        }
        catch(const std::runtime_error e) {
            error << "Failed to write output file!" << std::endl;
            throw std::runtime_error(error.str());
        }
    }

    void split_pixel_shader_collection_file(std::variant<std::filesystem::path, std::vector<char>> input, std::filesystem::path output_directory) {
        std::stringstream error;

        if(!std::filesystem::exists(output_directory)) {
            error << "Output directory '" << output_directory << "' does not exists!" << std::endl;
            throw std::runtime_error(error.str());
        }

        std::vector<char> input_data;
        if(std::holds_alternative<std::filesystem::path>(input)) {
            auto input_file = std::get<std::filesystem::path>(input);

            // Put the output files in a subfolder 
            output_directory /= input_file.stem();
            std::filesystem::create_directories(output_directory);
        
            try {
                input_data = read_file(input_file);
            }
            catch(const std::runtime_error e) {
                error << e.what() << std::endl;
                error << "Failed to read input file!" << std::endl;
                throw std::runtime_error(error.str());
            }
        }
        else {
            input_data = std::get<std::vector<char>>(input);

            // Create subfolder with a default name 
            output_directory /= "output";
            std::filesystem::create_directories(output_directory);
        }

        try {
            auto shaders_data = explode_pixel_shader_collection(input_data);
            for(auto &shader_data : shaders_data) {
                std::filesystem::create_directories(output_directory / shader_data.name);
                for(std::size_t function_index = 0; function_index < shader_data.functions.size(); function_index++) {
                    auto &function = shader_data.functions[function_index];
                    auto output_file_name = std::to_string(function_index) + "_" + function.name + ".bin";
                    auto output_file_path = output_directory / shader_data.name / output_file_name;
                    try {
                        write_file(output_file_path, function.data);
                    }
                    catch(const std::runtime_error e) {
                        error << e.what() << std::endl;
                        error << "Failed to write '" << output_file_path << "' file!" << std::endl;
                        throw std::runtime_error(error.str());
                    }
                }
            }
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to explode pixel shader collection!" << std::endl;
            throw std::runtime_error(error.str());
        }
    }

    vs_collection_data_t read_vertex_shader_collection_from_files(std::filesystem::path directory) {
        std::stringstream error;

        if(!std::filesystem::exists(directory)) {
            error << "Directory '" << directory << "' does not exists!" << std::endl;
            throw std::runtime_error(error.str());
        }

        std::vector<std::vector<char>> input_data;
        try {
            std::vector<std::filesystem::path> shader_files;
            std::copy(std::filesystem::directory_iterator(directory), std::filesystem::directory_iterator(), std::back_inserter(shader_files));
            std::sort(shader_files.begin(), shader_files.end(), numeric_compare);

            bool empty_input = true;
            for(auto &file_path : shader_files) {
                if(std::filesystem::is_regular_file(file_path) && file_path.extension() == ".bin") {
                    empty_input = false;
                    auto &shader_data = input_data.emplace_back();
                    shader_data = read_file(file_path);
                }
            }

            if(empty_input) {
                throw std::runtime_error("No shaders were found: invalid directory structure for vertex shader collection.");
            }
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to read input file!" << std::endl;
            throw std::runtime_error(error.str());
        }
        return std::move(input_data);
    }

    void create_vertex_shader_collection_file(std::variant<std::filesystem::path, vs_collection_data_t> input, std::filesystem::path output_file) {
        std::stringstream error;

        vs_collection_data_t input_data;
        if(std::holds_alternative<std::filesystem::path>(input)) {
            try {
                auto directory = std::get<std::filesystem::path>(input);
                input_data = read_vertex_shader_collection_from_files(directory);
            }
            catch(const std::runtime_error e) {
                throw;
            }
        }
        else {
            input_data = std::get<vs_collection_data_t>(input);
        }

        std::vector<char> output_data;
        try {
            output_data = merge_vertex_shaders(input_data);
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to merge pixel shader!" << std::endl;
            throw std::runtime_error(error.str());
        }

        try {
            write_file(output_file, output_data);
        }
        catch(const std::runtime_error e) {
            error << "Failed to write output file!" << std::endl;
            throw std::runtime_error(error.str());
        }
    }

    void split_vertex_shader_collection_file(std::variant<std::filesystem::path, std::vector<char>> input, std::filesystem::path output_directory) {
        std::stringstream error;

        if(!std::filesystem::exists(output_directory)) {
            error << "Output directory '" << output_directory << "' does not exists!" << std::endl;
            throw std::runtime_error(error.str());
        }

        std::vector<char> input_data;
        if(std::holds_alternative<std::filesystem::path>(input)) {
            auto input_file = std::get<std::filesystem::path>(input);

            // Put the output files in a subfolder 
            output_directory /= input_file.stem();
            std::filesystem::create_directories(output_directory);
        
            try {
                input_data = read_file(input_file);
            }
            catch(const std::runtime_error e) {
                error << e.what() << std::endl;
                error << "Failed to read input file!" << std::endl;
                throw std::runtime_error(error.str());
            }
        }
        else {
            input_data = std::get<std::vector<char>>(input);

            // Create subfolder with a default name 
            output_directory /= "output";
            std::filesystem::create_directories(output_directory);
        }

        try {
            auto shaders_data = explode_vertex_shader_collection(input_data);
            for(std::size_t i = 0; i < shaders_data.size(); i++) {
                auto &shader_data = shaders_data[i];
                auto output_file = output_directory / ("vs_chunk_" + std::to_string(i) + ".bin");
                try {
                    write_file(output_file, shader_data);
                }
                catch(const std::runtime_error e) {
                    error << e.what() << std::endl;
                    error << "Failed to write '" << output_file << "' file!" << std::endl;
                    throw std::runtime_error(error.str());
                }
            }
        }
        catch(const std::runtime_error e) {
            error << "Failed to explode vertex shader collection!" << std::endl;
            error << e.what() << std::endl;
            throw std::runtime_error(error.str());
        }
    }

    void decrypt_shader_file(std::filesystem::path input_file, std::filesystem::path output_file) {
        std::stringstream error;
        std::vector<char> input_data;
        std::vector<char> output_data;

        try {
            input_data = read_file(input_file);
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to read input file!" << std::endl;
            throw std::runtime_error(error.str());
        }

        try {
            output_data = decrypt_shader(input_data);
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to decrypt shader!" << std::endl;
            throw std::runtime_error(error.str());
        }

        try {
            write_file(output_file, output_data);
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to write output file!" << std::endl;
            throw std::runtime_error(error.str());
        }
    }

    void encrypt_shader_file(std::filesystem::path input_file, std::filesystem::path output_file) {
        std::stringstream error;
        std::vector<char> input_data;
        std::vector<char> output_data;
        
        try {
            input_data = read_file(input_file);
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to read input file!" << std::endl;
            throw std::runtime_error(error.str());
        }

        try {
            output_data = encrypt_shader(input_data);
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to encrypt shader!" << std::endl;
            throw std::runtime_error(error.str());
        }

        try {
            write_file(output_file, output_data);
        }
        catch(const std::runtime_error e) {
            error << e.what() << std::endl;
            error << "Failed to write output file!" << std::endl;
            throw std::runtime_error(error.str());
        }
    }
}
