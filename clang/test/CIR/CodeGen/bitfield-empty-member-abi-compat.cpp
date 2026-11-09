// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -std=c++20 -fclangir -emit-cir %s -o - | FileCheck %s
// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -std=c++20 -fclang-abi-compat=6 -fclangir -emit-cir %s -o - | FileCheck %s

// Older ABI modes consider this class empty, but its field still has storage.
// isZeroSize alone must not let the bit-field access unit overlap that field.
struct UnnamedBits {
  unsigned : 24;
};

struct BitsBeforeEmpty {
  UnnamedBits a;
  unsigned x : 24;
  [[no_unique_address]] UnnamedBits b;
};
static_assert(sizeof(BitsBeforeEmpty) == 12);
static_assert(__builtin_offsetof(BitsBeforeEmpty, b) == 7);
// CHECK: !rec_BitsBeforeEmpty = !cir.struct<"BitsBeforeEmpty" {data !rec_UnnamedBits, pad !u8i, bitfield !cir.bitfield<!cir.array<!u8i x 3>, [#cir.bitfield_decl<!u32i, 24>]>, data !rec_UnnamedBits, pad !cir.array<!u8i x 2>}>
BitsBeforeEmpty s;
