// SPDX-License-Identifier: GPL-3.0-only

#ifndef COMPOSER__ENCRYPT_HPP
#define COMPOSER__ENCRYPT_HPP

#include <vector>

namespace Composer {
    /**
     * Decrypt Halo's shader data
     * @param encrypted_shader_data     encrypted shader data
     * @return                          shader data
     */
    std::vector<char> decrypt_shader(std::vector<char> const &encrypted_shader_data);

    /**
     * Encrypt Halo's shader data
     * @param shader_data   shader data
     * @return              encrypted shader data
     */
    std::vector<char> encrypt_shader(std::vector<char> const &shader_data);
}

#endif
