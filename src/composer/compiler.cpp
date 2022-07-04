// SPDX-License-Identifier: GPL-3.0-only

#include <vector>
#include <sstream>
#include <filesystem>
#include <d3dx9shader.h>
#include <d3d9types.h>
#include <d3dcommon.h>
#include <composer/compiler.hpp>

namespace Composer {
    static std::stringstream last_error;

    struct ShaderCommentToken {
        std::uint16_t token;
        std::uint16_t size : 15;
        std::uint16_t zero : 1;
    };

    static std::string get_last_error() noexcept {
        auto error = last_error.str();
        std::stringstream().swap(last_error);
        return error;
    }

    std::vector<char> strip_shader_comments(std::vector<char> const &shader_data) {
        auto *comment_token = reinterpret_cast<ShaderCommentToken const *>(shader_data.data() + 4);
        if(comment_token->token == D3DSIO_COMMENT) {
            std::vector<char> output;

            // Shader version
            output.insert(output.begin(), shader_data.begin(), shader_data.begin() + 4);

            // Shader data
            std::size_t comment_begin = 8;
            std::size_t comment_end = comment_begin + comment_token->size * sizeof(std::uint32_t);
            output.insert(output.end(), shader_data.begin() + comment_end, shader_data.end());
        
            return std::move(output);
        }
        return shader_data;
    }

    std::vector<char> compile_shader(std::vector<char> const &shader_source, std::string entry_point, std::string model, bool strip_comments, LPD3DXINCLUDE include) {
        LPD3DXBUFFER shader_code = NULL;
        LPD3DXBUFFER errors = NULL;

        HRESULT res = D3DXCompileShader(shader_source.data(), shader_source.size(), NULL, include, entry_point.c_str(), model.c_str(), D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, &shader_code, &errors, NULL);
        if(res != D3D_OK) {
            last_error.write(reinterpret_cast<char *>(errors->GetBufferPointer()), errors->GetBufferSize());
            throw std::runtime_error(get_last_error());
        }

        std::vector<char> shader_data;
        auto *shader_code_buffer = reinterpret_cast<char *>(shader_code->GetBufferPointer());
        shader_data.insert(shader_data.begin(), shader_code_buffer, shader_code_buffer + shader_code->GetBufferSize());

        // Strip compiler comments from output data
        shader_data = strip_shader_comments(shader_data);

        return std::move(shader_data);
    }

    std::vector<char> assemble_shader(std::vector<char> const &shader_assembly) {
        LPD3DXBUFFER shader_code = NULL;
        LPD3DXBUFFER errors = NULL;

        HRESULT res = D3DXAssembleShader(shader_assembly.data(), shader_assembly.size(), NULL, NULL, 0, &shader_code, &errors);
        if(res != D3D_OK) {
            last_error.write(reinterpret_cast<char *>(errors->GetBufferPointer()), errors->GetBufferSize());
            throw std::runtime_error(get_last_error());
        }

        std::vector<char> shader_data;
        auto *shader_code_buffer = reinterpret_cast<char *>(shader_code->GetBufferPointer());
        shader_data.insert(shader_data.begin(), shader_code_buffer, shader_code_buffer + shader_code->GetBufferSize());

        return std::move(shader_data);
    }

    std::vector<char> disassemble_shader(std::vector<char> const &shader_bytecode) {
        constexpr const char *comment = "// Disassembled with Composer \n";
        
        LPD3DXBUFFER shader_asm;
        HRESULT res = D3DXDisassembleShader(reinterpret_cast<DWORD const *>(shader_bytecode.data()), FALSE, comment, &shader_asm);
        if(res != D3D_OK) {
            last_error << "Failed to disasseble shader" << std::endl;
            throw std::runtime_error(get_last_error());
        }

        std::vector<char> shader_asm_output;
        auto *shader_asm_buffer = reinterpret_cast<char *>(shader_asm->GetBufferPointer());
        shader_asm_output.insert(shader_asm_output.begin(), shader_asm_buffer, shader_asm_buffer + shader_asm->GetBufferSize() - 1);

        return std::move(shader_asm_output);
    }
}
