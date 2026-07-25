#pragma once

#include <array>
#include <cstdint>
#include <stdexcept>

#include "sbox.hpp"
#include "state.hpp"

namespace mini_aes {

/// Round constants rcon(i) from the Mini-AES key schedule (Section 3.6).
[[nodiscard]] inline constexpr Nibble round_constant(std::uint8_t round) noexcept {
    return Nibble{round};
}

/// Expands the 16-bit secret key into three round keys: K0, K1, K2.
///
/// Key schedule (Table 2):
///   K0 = (k0, k1, k2, k3)
///   w4 = w0 ⊕ S(w3) ⊕ rcon(1),  w5 = w1 ⊕ w4, ...
///   w8 = w4 ⊕ S(w7) ⊕ rcon(2),  w9 = w5 ⊕ w8, ...
class KeySchedule {
public:
    static constexpr std::size_t kRoundKeyCount = 3;

    explicit KeySchedule(std::uint16_t secret_key) {
        const auto& k = State::from_uint16(secret_key).nibbles();

        round_keys_[0] = State::from_nibbles(k[0], k[1], k[2], k[3]);

        const Nibble w4 = k[0] + sbox(k[3]) + round_constant(1);
        const Nibble w5 = k[1] + w4;
        const Nibble w6 = k[2] + w5;
        const Nibble w7 = k[3] + w6;
        round_keys_[1] = State::from_nibbles(w4, w5, w6, w7);

        const Nibble w8 = w4 + sbox(w7) + round_constant(2);
        const Nibble w9 = w5 + w8;
        const Nibble w10 = w6 + w9;
        const Nibble w11 = w7 + w10;
        round_keys_[2] = State::from_nibbles(w8, w9, w10, w11);
    }

    [[nodiscard]] const State& round_key(std::size_t round) const {
        if (round >= kRoundKeyCount) {
            throw std::out_of_range("Mini-AES supports round keys 0..2 only");
        }
        return round_keys_[round];
    }

    [[nodiscard]] std::uint16_t round_key_word(std::size_t round) const {
        return round_key(round).to_uint16();
    }

private:
    std::array<State, kRoundKeyCount> round_keys_{};
};

} // namespace mini_aes
