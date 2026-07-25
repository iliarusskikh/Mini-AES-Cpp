#pragma once

#include "gf16.hpp"
#include "sbox.hpp"
#include "state.hpp"

namespace mini_aes {

/// Applies the NibbleSub (γ) transformation to every nibble.
[[nodiscard]] inline State nibble_sub(const State& input, bool inverse = false) noexcept {
    State output;
    for (std::size_t index = 0; index < State::kNibbleCount; ++index) {
        const Nibble n = input.nibbles()[index];
        output.nibbles()[index] = inverse ? inv_sbox(n) : sbox(n);
    }
    return output;
}

/// Applies ShiftRow (π): (c0, c1, c2, c3) → (c0, c3, c2, c1).
/// Equivalent to swapping c1 and c3; self-inverse on this 2×2 layout.
[[nodiscard]] inline State shift_row(const State& input) noexcept {
    const auto& n = input.nibbles();
    return State::from_nibbles(n[0], n[3], n[2], n[1]);
}

namespace detail {

/// MixColumn constant matrix over GF(2^4):
///   | 3  2 |
///   | 2  3 |
/// This matrix is chosen so that MixColumn is its own inverse (θ = θ⁻¹).
inline constexpr std::array<std::array<Nibble, 2>, 2> kMixColumnMatrix = {{
    {{Nibble{3}, Nibble{2}}},
    {{Nibble{2}, Nibble{3}}},
}};

[[nodiscard]] inline Nibble dot_product(Nibble m0, Nibble m1, Nibble v0, Nibble v1) noexcept {
    return m0 * v0 + m1 * v1;
}

} // namespace detail

/// Applies MixColumn (θ) to each column independently.
[[nodiscard]] inline State mix_column(const State& input) noexcept {
    State output;
    for (std::size_t column = 0; column < 2; ++column) {
        const Nibble top = input.column_top(column);
        const Nibble bottom = input.column_bottom(column);

        output.column_top(column) =
            detail::dot_product(detail::kMixColumnMatrix[0][0],
                                detail::kMixColumnMatrix[0][1],
                                top,
                                bottom);
        output.column_bottom(column) =
            detail::dot_product(detail::kMixColumnMatrix[1][0],
                                detail::kMixColumnMatrix[1][1],
                                top,
                                bottom);
    }
    return output;
}

/// Applies KeyAddition (σ_K): XOR each nibble with the corresponding round-key nibble.
[[nodiscard]] inline State add_round_key(const State& input, const State& round_key) noexcept {
    State output;
    for (std::size_t index = 0; index < State::kNibbleCount; ++index) {
        output.nibbles()[index] = input.nibbles()[index] + round_key.nibbles()[index];
    }
    return output;
}

} // namespace mini_aes
