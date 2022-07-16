#include <cstdint>
#include <iostream>
#include <composer/encrypt.hpp>
#include <hash-library/md5.h>

namespace Composer {
    constexpr const std::uint32_t key[] = { 0x3FFFEF, 0xE5, 0x3FFFFFDD, 0x7FC3 };
    constexpr const std::uint32_t delta = 0x61C88647;
    
    std::vector<char> decrypt_shader(std::vector<char> const &encrypted_shader_data) {
        if(encrypted_shader_data.size() < 8) {
            throw std::runtime_error("shader data is too small");
        }

        auto decrypt_block = [](char *buffer) {
            std::uint32_t &slice_1 = *reinterpret_cast<std::uint32_t *>(buffer);
            std::uint32_t &slice_2 = *reinterpret_cast<std::uint32_t *>(buffer + 4);
            std::int32_t sum = 0xC6EF3720;

            for(std::size_t i = 0; i < 32; i++) {
                slice_2 -= (((slice_1 >> 0x5) + key[0]) ^ ((slice_1 << 0x4) + key[1])) ^ (sum + slice_1);
                slice_1 -= (((slice_2 << 0x4) + key[2]) ^ ((slice_2 >> 0x5) + key[3])) ^ (sum + slice_2);
                sum = static_cast<std::uint64_t>(sum) + delta;
            }
        };

        auto buffer = encrypted_shader_data;
        auto buffer_size = buffer.size();

        if(buffer_size % 8) {
            decrypt_block(buffer.data() + buffer_size - 8);
        }

        for(std::size_t i = 0; i < buffer_size / 8; i++) {
            decrypt_block(buffer.data() + i * 8);
        }

        // Decrypted data MD5 hash
        char hash[32];
        std::copy(buffer.end() - 33, buffer.end() - 1, hash);

        // Check if decrypted data is valid
        MD5 md5;
        if(md5(buffer.data(), buffer.size() - 33) != std::string(hash, 32)) {
            throw std::runtime_error("decrypted data checksum failed");
        }

        // Check if it is all good
        if(buffer.back() != 0) {
            throw std::runtime_error("decrypted data is not null terminated");
        }

        // Remove decrypted data checksum
        buffer.resize(buffer.size() - 33);

        return std::move(buffer);
    }

    std::vector<char> encrypt_shader(std::vector<char> const &shader_data) {
        if(shader_data.size() < 8) {
            throw std::runtime_error("shader data is too small");
        }

        auto encrypt_block = [](char *buffer) {
            std::uint32_t &slice_1 = *reinterpret_cast<std::uint32_t *>(buffer);
            std::uint32_t &slice_2 = *reinterpret_cast<std::uint32_t *>(buffer + 4);
            std::int32_t sum = 0;

            for(std::size_t i = 0; i < 32; i++) {
                sum = static_cast<std::uint64_t>(sum) - delta;
                slice_1 += (((slice_2 << 0x4) + key[2]) ^ ((slice_2 >> 0x5) + key[3])) ^ (sum + slice_2);
                slice_2 += (((slice_1 >> 0x5) + key[0]) ^ ((slice_1 << 0x4) + key[1])) ^ (sum + slice_1);
            }
        };

        // Hash shader data
        MD5 md5;
        std::string hash = md5(shader_data.data(), shader_data.size());

        auto buffer = shader_data;
        buffer.insert(buffer.end(), hash.begin(), hash.end());
        buffer.push_back(0); // all good
        
        auto buffer_size = buffer.size();

        for(std::size_t i = 0; i < buffer_size / 8; i++) {
            encrypt_block(buffer.data() + i * 8);
        }

        if(buffer_size % 8) {
            encrypt_block(buffer.data() + buffer_size - 8);
        }

        return std::move(buffer);
    }
}
