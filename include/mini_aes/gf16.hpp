#pragma once

#include <array>
#include <cstdint>

namespace mini_aes::detail {

/// Precomputed GF(2^4) multiplication table modulo x^4 + x + 1.
/// Built at compile time; O(1) lookup replaces the bit-serial multiply in the C version.
inline constexpr std::array<std::array<std::uint8_t, 16>, 16> build_gf16_mul_table() noexcept {
    std::array<std::array<std::uint8_t, 16>, 16> table{};
    constexpr std::uint8_t modulus = 0x13; // x^4 + x + 1

    for (std::uint8_t a = 0; a < 16; ++a) {
        for (std::uint8_t b = 0; b < 16; ++b) {
            std::uint8_t product = 0;
            std::uint8_t multiplicand = a;
            std::uint8_t multiplier = b;

            while (multiplier != 0) {
                if (multiplier & 1U) {
                    product ^= multiplicand;
                }
                multiplier >>= 1U;
                const bool carry = (multiplicand & 0x08U) != 0U;
                multiplicand <<= 1U;
                if (carry) {
                    multiplicand ^= modulus;
                }
            }

            table[a][b] = static_cast<std::uint8_t>(product & 0x0F);
        }
    }

    return table;
}

inline constexpr auto kGf16MulTable = build_gf16_mul_table();

} // namespace mini_aes::detail

namespace mini_aes {

/// A 4-bit nibble treated as an element of GF(2^4).
/// Addition in this field is bitwise XOR; multiplication uses the
/// irreducible polynomial m(x) = x^4 + x + 1 (0x13).
class Nibble {
public:
    static constexpr std::uint8_t kMask = 0x0F;

    constexpr Nibble() = default;

    explicit constexpr Nibble(std::uint8_t value) noexcept
        : value_(value & kMask) {}

    [[nodiscard]] constexpr std::uint8_t value() const noexcept { return value_; }

    [[nodiscard]] friend constexpr Nibble operator+(Nibble lhs, Nibble rhs) noexcept {
        return Nibble{static_cast<std::uint8_t>(lhs.value_ ^ rhs.value_)};
    }

    Nibble& operator+=(Nibble rhs) noexcept {
        value_ ^= rhs.value_;
        return *this;
    }

    [[nodiscard]] friend constexpr Nibble operator*(Nibble lhs, Nibble rhs) noexcept {
        return Nibble{detail::kGf16MulTable[lhs.value_][rhs.value_]};
    }

private:
    std::uint8_t value_ = 0;
};

} // namespace mini_aes
