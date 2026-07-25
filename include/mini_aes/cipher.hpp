#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <string_view>

#include "key_schedule.hpp"
#include "transforms.hpp"

namespace mini_aes {

/// Mini-AES block cipher (16-bit block, 16-bit key, 2 rounds).
///
/// Encryption (Section 3.7):
///   E = σ_K2 ∘ π ∘ γ ∘ σ_K1 ∘ θ ∘ π ∘ γ ∘ σ_K0
///
/// Decryption (Section 3.8):
///   D = σ_K0 ∘ π ∘ γ⁻¹ ∘ θ ∘ σ_K1 ∘ π ∘ γ⁻¹ ∘ σ_K2
class Cipher {
public:
    using TraceCallback = std::function<void(std::string_view label, const State& state)>;

    explicit Cipher(std::uint16_t secret_key) : schedule_(secret_key) {}

    [[nodiscard]] std::uint16_t encrypt(std::uint16_t plaintext, TraceCallback trace = {}) const {
        State state = State::from_uint16(plaintext);
        trace_step(trace, "Input plaintext", state);

        state = add_round_key(state, schedule_.round_key(0));
        trace_step(trace, "Round 0: KeyAddition", state);

        state = nibble_sub(state);
        trace_step(trace, "Round 1: NibbleSub", state);

        state = shift_row(state);
        trace_step(trace, "Round 1: ShiftRow", state);

        state = mix_column(state);
        trace_step(trace, "Round 1: MixColumn", state);

        state = add_round_key(state, schedule_.round_key(1));
        trace_step(trace, "Round 1: KeyAddition", state);

        state = nibble_sub(state);
        trace_step(trace, "Round 2: NibbleSub", state);

        state = shift_row(state);
        trace_step(trace, "Round 2: ShiftRow", state);

        // Final round omits MixColumn, mirroring full AES structure.
        state = add_round_key(state, schedule_.round_key(2));
        trace_step(trace, "Round 2: KeyAddition (ciphertext)", state);

        return state.to_uint16();
    }

    [[nodiscard]] std::uint16_t decrypt(std::uint16_t ciphertext, TraceCallback trace = {}) const {
        State state = State::from_uint16(ciphertext);
        trace_step(trace, "Input ciphertext", state);

        state = add_round_key(state, schedule_.round_key(2));
        trace_step(trace, "Inverse round 2: KeyAddition", state);

        state = shift_row(state);
        trace_step(trace, "Inverse round 2: ShiftRow", state);

        state = nibble_sub(state, /*inverse=*/true);
        trace_step(trace, "Inverse round 2: InvNibbleSub", state);

        state = add_round_key(state, schedule_.round_key(1));
        trace_step(trace, "Inverse round 1: KeyAddition", state);

        state = mix_column(state);
        trace_step(trace, "Inverse round 1: MixColumn", state);

        state = shift_row(state);
        trace_step(trace, "Inverse round 1: ShiftRow", state);

        state = nibble_sub(state, /*inverse=*/true);
        trace_step(trace, "Inverse round 1: InvNibbleSub", state);

        state = add_round_key(state, schedule_.round_key(0));
        trace_step(trace, "Inverse round 0: KeyAddition (plaintext)", state);

        return state.to_uint16();
    }

    [[nodiscard]] const KeySchedule& key_schedule() const noexcept { return schedule_; }

private:
    static void trace_step(const TraceCallback& trace, std::string_view label, const State& state) {
        if (trace) {
            trace(label, state);
        }
    }

    KeySchedule schedule_;
};

} // namespace mini_aes
