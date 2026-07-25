#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>

#include "mini_aes/cipher.hpp"

namespace {

void print_header() {
    std::cout << "Mini-AES (C++ implementation)\n"
              << "Educational 16-bit block cipher by Raphael Phan (Cryptologia, 2002)\n\n";
}

void print_hex_block(std::string_view label, std::uint16_t value) {
    std::cout << label << ": 0x" << std::uppercase << std::hex << std::setw(4)
              << std::setfill('0') << value << std::dec << std::nouppercase << '\n';
}

void trace_state(std::string_view label, const mini_aes::State& state) {
    state.print_trace(label);
}

} // namespace, private to this file

int main() {
    print_header();

    // Example 9 from the Mini-AES paper:
    //   P = 1001 1100 0110 0011 (0x9C63)
    //   K = 1100 0011 1111 0000 (0xC3F0)
    //   C = 0111 0010 1100 0110 (0x72C6)
    constexpr std::uint16_t plaintext = 0x9C63;
    constexpr std::uint16_t secret_key = 0xC3F0;
    constexpr std::uint16_t expected_ciphertext = 0x72C6;

    print_hex_block("Plaintext", plaintext);
    print_hex_block("Secret key", secret_key);

    const mini_aes::Cipher cipher(secret_key);

    std::cout << "\nRound keys:\n";
    for (std::size_t round = 0; round < mini_aes::KeySchedule::kRoundKeyCount; ++round) {
        print_hex_block("  K" + std::to_string(round), cipher.key_schedule().round_key_word(round));
    }

    std::cout << "\nEncryption trace\n";
    std::cout << "----------------\n";
    const std::uint16_t ciphertext = cipher.encrypt(plaintext, trace_state);
    print_hex_block("Ciphertext", ciphertext);
    std::cout << (ciphertext == expected_ciphertext ? "PASS: matches paper Example 9\n"
                                                    : "FAIL: ciphertext mismatch\n");

    std::cout << "\nDecryption trace\n";
    std::cout << "----------------\n";
    const std::uint16_t recovered = cipher.decrypt(ciphertext, trace_state);
    print_hex_block("Recovered plaintext", recovered);
    std::cout << (recovered == plaintext ? "PASS: decryption round-trip\n"
                                         : "FAIL: decryption mismatch\n");

    return (ciphertext == expected_ciphertext && recovered == plaintext) ? 0 : 1;
}
