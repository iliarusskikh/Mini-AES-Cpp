#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <string_view>

#include "gf16.hpp"

namespace mini_aes {

/// 2×2 state matrix of GF(2^4) nibbles in Mini-AES column layout.
///
/// A 16-bit block is four nibbles (c0, c1, c2, c3) packed MSB → LSB.
/// They are arranged for MixColumn/ShiftRow as:
///
///   | c0  c2 |
///   | c1  c3 |
///
/// This matches Figures 2–7 in Phan's Mini-AES paper (Cryptologia, 2002).
class State {
public:
    static constexpr std::size_t kNibbleCount = 4;

    constexpr State() = default;

    explicit constexpr State(std::array<Nibble, kNibbleCount> nibbles) noexcept
        : nibbles_(nibbles) {}

    [[nodiscard]] static constexpr State from_uint16(std::uint16_t block) noexcept {
        return State{{Nibble{static_cast<std::uint8_t>(block >> 12)},
                      Nibble{static_cast<std::uint8_t>((block >> 8) & 0x0F)},
                      Nibble{static_cast<std::uint8_t>((block >> 4) & 0x0F)},
                      Nibble{static_cast<std::uint8_t>(block & 0x0F)}}};
    }

    [[nodiscard]] static constexpr State from_nibbles(Nibble c0,
                                                      Nibble c1,
                                                      Nibble c2,
                                                      Nibble c3) noexcept {
        return State{{c0, c1, c2, c3}};
    }

    [[nodiscard]] constexpr std::uint16_t to_uint16() const noexcept {
        return static_cast<std::uint16_t>((nibbles_[0].value() << 12) |
                                          (nibbles_[1].value() << 8) |
                                          (nibbles_[2].value() << 4) |
                                          nibbles_[3].value());
    }

    /// Column index 0 → (c0, c1); column index 1 → (c2, c3).
    [[nodiscard]] constexpr Nibble column_top(std::size_t column) const noexcept {
        return nibbles_[column * 2];
    }

    [[nodiscard]] constexpr Nibble column_bottom(std::size_t column) const noexcept {
        return nibbles_[column * 2 + 1];
    }

    [[nodiscard]] constexpr Nibble& column_top(std::size_t column) noexcept {
        return nibbles_[column * 2];
    }

    [[nodiscard]] constexpr Nibble& column_bottom(std::size_t column) noexcept {
        return nibbles_[column * 2 + 1];
    }

    [[nodiscard]] constexpr const std::array<Nibble, kNibbleCount>& nibbles() const noexcept {
        return nibbles_;
    }

    [[nodiscard]] constexpr std::array<Nibble, kNibbleCount>& nibbles() noexcept {
        return nibbles_;
    }

    void print(std::ostream& out = std::cout) const {
        out << static_cast<unsigned>(nibbles_[0].value()) << ' '
            << static_cast<unsigned>(nibbles_[2].value()) << '\n'
            << static_cast<unsigned>(nibbles_[1].value()) << ' '
            << static_cast<unsigned>(nibbles_[3].value()) << '\n';
    }

    void print_trace(std::string_view label, std::ostream& out = std::cout) const {
        out << label << '\n';
        print(out);
    }

private:
    std::array<Nibble, kNibbleCount> nibbles_{};
};

} // namespace mini_aes
