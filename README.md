# Mini-AES

A modern **C++20** implementation of **Mini-AES** — a compact, educational 16-bit block cipher introduced by Raphael Phan in [*Mini Advanced Encryption Standard (Mini-AES): A Testbed for Cryptanalysis Students*](https://people.utm.my/rashidah/wp-content/uploads/sites/729/2017/03/mini-aes-spec.pdf) (Cryptologia, 2002).

Mini-AES preserves the structure of the real AES (Rijndael) while shrinking every parameter so the algorithm can be traced by hand. **It is not secure for production use.**

This repository is a typed, header-only C++ rewrite with encryption, decryption, and a step-by-step trace demo.

---

## Docs

| Document | Purpose |
|----------|---------|
| [PLAN.md](PLAN.md) | What the simulation does: data model, key schedule, encrypt/decrypt steps, demo flow |
| [theory/Theory_CodeRef.html](theory/Theory_CodeRef.html) | Lookup notes for C++ concepts and design decisions used in this codebase |

---

## What Mini-AES Does

| Property | Value |
|----------|-------|
| Block size | 16 bits (four 4-bit nibbles) |
| Key size | 16 bits |
| Rounds | 2 (+ initial/final key addition) |
| Field | GF(2⁴) with irreducible polynomial `x⁴ + x + 1` |

Each 16-bit block is viewed as a 2×2 nibble matrix:

```
| c0  c2 |
| c1  c3 |
```

The four round transformations (mirroring AES) are:

| Step | Symbol | Description |
|------|--------|-------------|
| **NibbleSub** | γ | Substitute each nibble through a 16-entry S-box (first row of the DES S-box) |
| **ShiftRow** | π | Swap nibbles `c1` and `c3` → `(c0, c3, c2, c1)` |
| **MixColumn** | θ | Multiply each column by the circulant matrix `[[3,2],[2,3]]` over GF(2⁴) |
| **KeyAddition** | σ_K | XOR the state with a round key |

**Encryption** (two rounds, no MixColumn in the last round):

```
E = σ_K2 ∘ π ∘ γ ∘ σ_K1 ∘ θ ∘ π ∘ γ ∘ σ_K0
```

**Decryption** uses inverse NibbleSub and the same ShiftRow / MixColumn (both self-inverse on this layout):

```
D = σ_K0 ∘ π ∘ γ⁻¹ ∘ θ ∘ σ_K1 ∘ π ∘ γ⁻¹ ∘ σ_K2
```

---

## Project Layout

```
include/mini_aes/
  gf16.hpp          # GF(2⁴) nibble type with compile-time multiply table
  sbox.hpp          # Forward / inverse S-boxes
  state.hpp         # 2×2 state matrix and 16-bit block packing
  transforms.hpp    # NibbleSub, ShiftRow, MixColumn, KeyAddition
  key_schedule.hpp  # Derives round keys K0, K1, K2
  cipher.hpp        # High-level encrypt / decrypt API
  mini_aes.hpp      # Convenience umbrella include

source/
  main.cpp          # Demo: paper Example 9 with trace output

theory/
  Theory_CodeRef.html  # C++ theory lookup tied to this code

CMakeLists.txt
PLAN.md             # Process guide for the simulation
LICENSE
README.md
```

---

## Build & Run

Requires a C++20 compiler and CMake 3.16+.

```bash
cmake -S . -B build
cmake --build build
./build/mini_aes
```

The demo encrypts and decrypts the **Example 9** test vector from the paper:

| | Value |
|---|-------|
| Plaintext | `0x9C63` |
| Secret key | `0xC3F0` |
| Ciphertext | `0x72C6` |

---

## C++ Design Notes

Design choices in this implementation:

- **Encapsulates state** in a `State` class with the paper's column-oriented matrix layout
- **Uses `constexpr` lookup tables** for GF(2⁴) multiplication instead of runtime bit-serial polynomial math
- **Separates concerns** into focused headers (field arithmetic, transforms, key schedule, cipher)
- **Provides type-safe `Nibble` values** instead of raw `uint8_t` with manual masking
- **Implements full decryption**, not just encryption
- **Supports optional trace callbacks** for teaching / debugging intermediate states

For a concept-by-concept walkthrough, open [theory/Theory_CodeRef.html](theory/Theory_CodeRef.html).

### Quick API Example

```cpp
#include "mini_aes/cipher.hpp"

mini_aes::Cipher cipher(0xC3F0);
std::uint16_t ciphertext = cipher.encrypt(0x9C63);
std::uint16_t plaintext  = cipher.decrypt(ciphertext);
```

With step-by-step tracing:

```cpp
cipher.encrypt(0x9C63, [](std::string_view label, const mini_aes::State& state) {
    state.print_trace(label);
});
```

---

## Reference

- Phan, R. C.-W. (2002). *Mini Advanced Encryption Standard (Mini-AES): A Testbed for Cryptanalysis Students.* Cryptologia, 26(4). Spec PDF: [mini-aes-spec.pdf](https://people.utm.my/rashidah/wp-content/uploads/sites/729/2017/03/mini-aes-spec.pdf).
- The paper also describes the **Square attack** on an extended 4-round variant — useful for cryptanalysis coursework.

---

## License

See [LICENSE](LICENSE).
