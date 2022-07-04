// SPDX-License-Identifier: GPL-3.0-only

#ifndef COMPOSER__COMPILER_HPP
#define COMPOSER__COMPILER_HPP

#include <string>
#include <vector>
#include <d3dx9shader.h>

namespace Composer {
    /**
     * Remove shader comments
     * @param shader_data   compiled shader data
     * @return              shader without that ugly comment block
     */
    std::vector<char> strip_shader_comments(std::vector<char> const &shader_data);

    /**
     * Compile shader from source
     * @param shader_source     shader source data
     * @param entry_point       name of the function where execution begins
     * @param model             shader model; instruction set that will be used
     * @param strip_comments    strip compiler comments from shader output data
     * @return                  compiled shader data
     * @exception               if compilation process fails
     */
    std::vector<char> compile_shader(std::vector<char> const &shader_source, std::string entry_point, std::string model, bool strip_comments = false, LPD3DXINCLUDE include = NULL);

    /**
     * Assemble shader from assembly source
     * @param shader_assembly   shader assembly code data
     * @return                  assembled shader data
     * @exception               if assemble process fails
     */
    std::vector<char> assemble_shader(std::vector<char> const &shader_assembly);

    /**
     * Disassemble compiled shader
     * @param shader_bytecode   shader byte code data
     * @return                  disassembled shader data
     * @exception               if disassemble process fails
     */
    std::vector<char> disassemble_shader(std::vector<char> const &shader_bytecode);
}

#endif
