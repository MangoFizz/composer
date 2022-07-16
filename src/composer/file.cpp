// SPDX-License-Identifier: GPL-3.0-only

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <composer/encrypt.hpp>

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
