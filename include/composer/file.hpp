// SPDX-License-Identifier: GPL-3.0-only

#ifndef COMPOSER__FILE_HPP
#define COMPOSER__FILE_HPP

#include <filesystem>

namespace Composer {
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
