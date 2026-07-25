#pragma once

#include <array>
#include <cstdint>

#include "gf16.hpp"

namespace mini_aes {

/// Substitution boxes from Phan's Mini-AES specification (Cryptologia, 2002).
/// Values are taken from the first row of the DES S-box.
inline constexpr std::array<std::uint8_t, 16> kSBox = {
    0xE, 0x4, 0xD, 0x1, 0x2, 0xF, 0xB, 0x8,
    0x3, 0xA, 0x6, 0xC, 0x5, 0x9, 0x0, 0x7,
};

/// Inverse S-box used during decryption (Table 3 in the paper).
inline constexpr std::array<std::uint8_t, 16> build_inv_sbox() noexcept {
    std::array<std::uint8_t, 16> inverse{};
    for (std::uint8_t input = 0; input < 16; ++input) {
        inverse[kSBox[input]] = input;
    }
    return inverse;
}

inline constexpr std::array<std::uint8_t, 16> kInvSBox = build_inv_sbox();

[[nodiscard]] inline constexpr Nibble sbox(Nibble n) noexcept {
    return Nibble{kSBox[n.value()]};
}

[[nodiscard]] inline constexpr Nibble inv_sbox(Nibble n) noexcept {
    return Nibble{kInvSBox[n.value()]};
}

} // namespace mini_aes
