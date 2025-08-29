/*
 * Copyright (c) 2021, SCNUCrypto, South China Normal University
 *
 * Apache License 2.0
 *
 */
#ifndef _HOPTWBMATRIX_H_
#define _HOPTWBMATRIX_H_
/***
 * Last Update: 2022/02/20
 * Version: 3.3.0
 ***/
#include "OptWBMatrix/structure.h"
#include "OptWBMatrix/random.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void opt_SetRandSeed(unsigned int seed); // Set random seed
    /*
     * 4-bit Matrix operation
     */

    void opt_initM4(M4 *Mat);
    void opt_randM4(M4 *Mat);
    void opt_identityM4(M4 *Mat);
    void opt_printM4(M4 Mat);
    void opt_printbitM4(M4 Mat);
    void opt_copyM4(M4 Mat1, M4 *Mat2);
    int opt_isequalM4(M4 Mat1, M4 Mat2);
    int opt_isinvertM4(M4 Mat);
    void opt_invsM4(M4 Mat, M4 *Mat_inv);
    int opt_readbitM4(M4 Mat, int i, int j);
    void opt_flipbitM4(M4 *Mat, int i, int j);
    void opt_setbitM4(M4 *Mat, int i, int j, int bit);

    void opt_initV4(V4 *Vec);
    void opt_randV4(V4 *Vec);
    void opt_printV4(V4 Vec);
    int opt_isequalV4(V4 Vec1, V4 Vec2);
    void opt_VecAddVecV4(V4 Vec1, V4 Vec2, V4 *Vec);

    uint8_t opt_affineU4(Aff4 aff, uint8_t arr);
    int opt_parityU4(uint8_t n);
    int opt_HWU4(uint8_t n);

    void opt_MatMulVecM4(M4 Mat, V4 Vec, V4 *ans);
    uint8_t opt_MatMulNumM4(M4 Mat, uint8_t n);
    void opt_MatMulMatM4(M4 Mat1, M4 Mat2, M4 *Mat);
    void opt_MatAddMatM4(M4 Mat1, M4 Mat2, M4 *Mat);
    void opt_MattransM4(M4 Mat, M4 *Mat_trans);

    void opt_genMatpairM4(M4 *Mat, M4 *Mat_inv);
    void opt_genaffinepairM4(Aff4 *aff, Aff4 *aff_inv);
    void opt_affinemixM4(Aff4 aff, Aff4 preaff_inv, Aff4 *mixaff);

    /*
     * 8-bit Matrix operation
     */

    void opt_initM8(M8 *Mat);
    void opt_randM8(M8 *Mat);
    void opt_identityM8(M8 *Mat);
    void opt_printM8(M8 Mat);
    void opt_printbitM8(M8 Mat);
    void opt_copyM8(M8 Mat1, M8 *Mat2);
    int opt_isequalM8(M8 Mat1, M8 Mat2);
    int opt_isinvertM8(M8 Mat);
    void opt_invsM8(M8 Mat, M8 *Mat_inv);
    int opt_readbitM8(M8 Mat, int i, int j);
    void opt_flipbitM8(M8 *Mat, int i, int j);
    void opt_setbitM8(M8 *Mat, int i, int j, int bit);

    void opt_initV8(V8 *Vec);
    void opt_randV8(V8 *Vec);
    void opt_printV8(V8 Vec);
    int opt_isequalV8(V8 Vec1, V8 Vec2);
    void opt_VecAddVecV8(V8 Vec1, V8 Vec2, V8 *Vec);

    uint8_t opt_affineU8(Aff8 aff, uint8_t arr);
    int opt_parityU8(uint8_t n);
    int opt_HWU8(uint8_t n);
    void opt_printU8(uint8_t n);

    void opt_MatMulVecM8(M8 Mat, V8 Vec, V8 *ans);
    uint8_t opt_MatMulNumM8(M8 Mat, uint8_t n);
    void opt_MatMulMatM8(M8 Mat1, M8 Mat2, M8 *Mat);
    void opt_MatAddMatM8(M8 Mat1, M8 Mat2, M8 *Mat);
    void opt_MattransM8(M8 Mat, M8 *Mat_trans);

    void opt_genMatpairM8(M8 *Mat, M8 *Mat_inv);
    void opt_genaffinepairM8(Aff8 *aff, Aff8 *aff_inv);
    void opt_affinemixM8(Aff8 aff, Aff8 preaff_inv, Aff8 *mixaff);

    /*
     * 16-bit Matrix operation
     */

    void opt_initM16(M16 *Mat);
    void opt_randM16(M16 *Mat);
    void opt_identityM16(M16 *Mat);
    void opt_printM16(M16 Mat);
    void opt_printbitM16(M16 Mat);
    void opt_copyM16(M16 Mat1, M16 *Mat2);
    int opt_isequalM16(M16 Mat1, M16 Mat2);
    int opt_isinvertM16(M16 Mat);
    void opt_invsM16(M16 Mat, M16 *Mat_inv);
    int opt_readbitM16(M16 Mat, int i, int j);
    void opt_flipbitM16(M16 *Mat, int i, int j);
    void opt_setbitM16(M16 *Mat, int i, int j, int bit);

    void opt_initV16(V16 *Vec);
    void opt_randV16(V16 *Vec);
    void opt_printV16(V16 Vec);
    int opt_isequalV16(V16 Vec1, V16 Vec2);
    void opt_VecAddVecV16(V16 Vec1, V16 Vec2, V16 *Vec);

    uint16_t opt_affineU16(Aff16 aff, uint16_t arr);
    int opt_parityU16(uint16_t n);
    int opt_HWU16(uint16_t n);
    void opt_printU16(uint16_t n);
    void opt_MatAddMatM16(M16 Mat1, M16 Mat2, M16 *Mat);
    void opt_MatMulVecM16(M16 Mat, V16 Vec, V16 *ans);
    uint16_t opt_MatMulNumM16(M16 Mat, uint16_t n);
    void opt_MatMulMatM16(M16 Mat1, M16 Mat2, M16 *Mat);
    void opt_MattransM16(M16 Mat, M16 *Mat_trans);

    void opt_genMatpairM16(M16 *Mat, M16 *Mat_inv);
    void opt_genaffinepairM16(Aff16 *aff, Aff16 *aff_inv);
    void opt_affinemixM16(Aff16 aff, Aff16 preaff_inv, Aff16 *mixaff);

    /*
     * 32-bit Matrix operation
     */

    void opt_initM32(M32 *Mat);
    void opt_randM32(M32 *Mat);
    void opt_identityM32(M32 *Mat);
    void opt_printM32(M32 Mat);
    void opt_printbitM32(M32 Mat);
    void opt_copyM32(M32 Mat1, M32 *Mat2);
    int opt_isequalM32(M32 Mat1, M32 Mat2);
    int opt_isinvertM32(M32 Mat);
    void opt_invsM32(M32 Mat, M32 *Mat_inv);
    int opt_readbitM32(M32 Mat, int i, int j);
    void opt_flipbitM32(M32 *Mat, int i, int j);
    void opt_setbitM32(M32 *Mat, int i, int j, int bit);

    void opt_initV32(V32 *Vec);
    void opt_randV32(V32 *Vec);
    void opt_printV32(V32 Vec);
    int opt_isequalV32(V32 Vec1, V32 Vec2);
    void opt_VecAddVecV32(V32 Vec1, V32 Vec2, V32 *Vec);

    uint32_t opt_affineU32(Aff32 aff, uint32_t arr);
    int opt_parityU32(uint32_t n);
    int opt_HWU32(uint32_t n);
    void opt_printU32(uint32_t n);

    void opt_MatMulVecM32(M32 Mat, V32 Vec, V32 *ans);
    uint32_t opt_MatMulNumM32(M32 Mat, uint32_t n);
    void opt_MatMulMatM32(M32 Mat1, M32 Mat2, M32 *Mat);
    void opt_MatAddMatM32(M32 Mat1, M32 Mat2, M32 *Mat);
    void opt_MattransM32(M32 Mat, M32 *Mat_trans);

    void opt_genMatpairM32(M32 *Mat, M32 *Mat_inv);
    void opt_genaffinepairM32(Aff32 *aff, Aff32 *aff_inv);
    void opt_affinemixM32(Aff32 aff, Aff32 preaff_inv, Aff32 *mixaff);
    void opt_MatrixcomM8to32(M8 m1, M8 m2, M8 m3, M8 m4, M32 *mat);
    void opt_VectorcomV8to32(V8 v1, V8 v2, V8 v3, V8 v4, V32 *vec);
    void opt_affinecomM8to32(Aff8 aff1, Aff8 aff2, Aff8 aff3, Aff8 aff4, Aff32 *aff);

    /*
     * 64-bit Matrix operation
     */

    void opt_initM64(M64 *Mat);
    void opt_randM64(M64 *Mat);
    void opt_identityM64(M64 *Mat);
    void opt_printM64(M64 Mat);
    void opt_printbitM64(M64 Mat);
    void opt_copyM64(M64 Mat1, M64 *Mat2);
    int opt_isequalM64(M64 Mat1, M64 Mat2);
    int opt_isinvertM64(M64 Mat);
    void opt_invsM64(M64 Mat, M64 *Mat_inv);
    int opt_readbitM64(M64 Mat, int i, int j);
    void opt_flipbitM64(M64 *Mat, int i, int j);
    void opt_setbitM64(M64 *Mat, int i, int j, int bit);

    void opt_initV64(V64 *Vec);
    void opt_randV64(V64 *Vec);
    void opt_printV64(V64 Vec);
    int opt_isequalV64(V64 Vec1, V64 Vec2);
    void opt_VecAddVecV64(V64 Vec1, V64 Vec2, V64 *Vec);

    uint64_t opt_affineU64(Aff64 aff, uint64_t arr);
    int opt_parityU64(uint64_t n);
    int opt_HWU64(uint64_t n);
    void opt_printU64(uint64_t n);

    void opt_MatMulVecM64(M64 Mat, V64 Vec, V64 *ans);
    uint64_t opt_MatMulNumM64(M64 Mat, uint64_t n);
    void opt_MatMulMatM64(M64 Mat1, M64 Mat2, M64 *Mat);
    void opt_MattransM64(M64 Mat, M64 *Mat_trans);

    void opt_MatAddMatM64(M64 Mat1, M64 Mat2, M64 *Mat);
    void opt_genMatpairM64(M64 *Mat, M64 *Mat_inv);
    void opt_genaffinepairM64(Aff64 *aff, Aff64 *aff_inv);
    void opt_affinemixM64(Aff64 aff, Aff64 preaff_inv, Aff64 *mixaff);

    void opt_MatrixcomM16to64(M16 m1, M16 m2, M16 m3, M16 m4, M64 *mat);
    void opt_VectorcomV16to64(V16 v1, V16 v2, V16 v3, V16 v4, V64 *vec);
    void opt_affinecomM16to64(Aff16 aff1, Aff16 aff2, Aff16 aff3, Aff16 aff4, Aff64 *aff);
    void opt_MatrixcomM8to64(M8 m1, M8 m2, M8 m3, M8 m4, M8 m5, M8 m6, M8 m7, M8 m8, M64 *mat);
    void opt_VectorcomV8to64(V8 v1, V8 v2, V8 v3, V8 v4, V8 v5, V8 v6, V8 v7, V8 v8, V64 *vec);
    void opt_affinecomM8to64(Aff8 aff1, Aff8 aff2, Aff8 aff3, Aff8 aff4, Aff8 aff5, Aff8 aff6, Aff8 aff7, Aff8 aff8, Aff64 *aff);

    /*
     * 128-bit Matrix operation
     */

    void opt_initM128(M128 *Mat);
    void opt_randM128(M128 *Mat);
    void opt_identityM128(M128 *Mat);
    void opt_printM128(M128 Mat);
    void opt_printbitM128(M128 Mat);
    void opt_copyM128(M128 Mat1, M128 *Mat2);
    int opt_isequalM128(M128 Mat1, M128 Mat2);
    int opt_isinvertM128(M128 Mat);
    void opt_invsM128(M128 Mat, M128 *Mat_inv);
    int opt_readbitM128(M128 Mat, int i, int j);
    void opt_flipbitM128(M128 *Mat, int i, int j);
    void opt_setbitM128(M128 *Mat, int i, int j, int bit);

    void opt_initV128(V128 *Vec);
    void opt_randV128(V128 *Vec);
    void opt_printV128(V128 Vec);

    void opt_affineU128(Aff128 aff, uint64_t arr[], uint64_t ans[]);
    int opt_parityU128(uint64_t n[]);
    int opt_HWU128(uint64_t n[]);
    void opt_printU128(uint64_t n[]);
    int opt_isequalV128(V128 Vec1, V128 Vec2);
    void opt_VecAddVecV128(V128 Vec1, V128 Vec2, V128 *Vec);

    void opt_MatMulVecM128(M128 Mat, V128 Vec, V128 *ans);
    void opt_MatMulMatM128(M128 Mat1, M128 Mat2, M128 *Mat);
    void opt_MattransM128(M128 Mat, M128 *Mat_trans);

    void opt_MatAddMatM128(M128 Mat1, M128 Mat2, M128 *Mat);
    void opt_genMatpairM128(M128 *Mat, M128 *Mat_inv);
    void opt_genaffinepairM128(Aff128 *aff, Aff128 *aff_inv);
    void opt_affinemixM128(Aff128 aff, Aff128 preaff_inv, Aff128 *mixaff);

    void opt_MatrixcomM32to128(M32 m1, M32 m2, M32 m3, M32 m4, M128 *mat);
    void opt_VectorcomV32to128(V32 v1, V32 v2, V32 v3, V32 v4, V128 *vec);
    void opt_affinecomM32to128(Aff32 aff1, Aff32 aff2, Aff32 aff3, Aff32 aff4, Aff128 *aff);
    void opt_MatrixcomM8to128(M8 m1, M8 m2, M8 m3, M8 m4, M8 m5, M8 m6, M8 m7, M8 m8, M8 m9, M8 m10, M8 m11, M8 m12, M8 m13, M8 m14, M8 m15, M8 m16, M128 *mat);
    void opt_VectorcomV8to128(V8 v1, V8 v2, V8 v3, V8 v4, V8 v5, V8 v6, V8 v7, V8 v8, V8 v9, V8 v10, V8 v11, V8 v12, V8 v13, V8 v14, V8 v15, V8 v16, V128 *vec);
    void opt_affinecomM8to128(Aff8 aff1, Aff8 aff2, Aff8 aff3, Aff8 aff4, Aff8 aff5, Aff8 aff6, Aff8 aff7, Aff8 aff8, Aff8 aff9, Aff8 aff10, Aff8 aff11, Aff8 aff12, Aff8 aff13, Aff8 aff14, Aff8 aff15, Aff8 aff16, Aff128 *aff);
    void opt_MatrixcomM16to128(M16 m1, M16 m2, M16 m3, M16 m4, M16 m5, M16 m6, M16 m7, M16 m8, M128 *mat);
    void opt_VectorcomV16to128(V16 v1, V16 v2, V16 v3, V16 v4, V16 v5, V16 v6, V16 v7, V16 v8, V128 *vec);
    void opt_affinecomM16to128(Aff16 aff1, Aff16 aff2, Aff16 aff3, Aff16 aff4, Aff16 aff5, Aff16 aff6, Aff16 aff7, Aff16 aff8, Aff128 *aff);

    /*
     * 256-bit Matrix operation
     */

    void opt_initM256(M256 *Mat);
    void opt_randM256(M256 *Mat);
    void opt_identityM256(M256 *Mat);
    void opt_printM256(M256 Mat);
    void opt_printbitM256(M256 Mat);
    void opt_copyM256(M256 Mat1, M256 *Mat2);
    int opt_isequalM256(M256 Mat1, M256 Mat2);
    int opt_isinvertM256(M256 Mat);
    void opt_invsM256(M256 Mat, M256 *Mat_inv);
    int opt_readbitM256(M256 Mat, int i, int j);
    void opt_flipbitM256(M256 *Mat, int i, int j);
    void opt_setbitM256(M256 *Mat, int i, int j, int bit);

    void opt_initV256(V256 *Vec);
    void opt_randV256(V256 *Vec);
    void opt_printV256(V256 Vec);

    void opt_affineU256(Aff256 aff, uint64_t arr[], uint64_t ans[]);
    int opt_parityU256(uint64_t n[]);
    int opt_HWU256(uint64_t n[]);
    void opt_printU256(uint64_t n[]);
    int opt_isequalV256(V256 Vec1, V256 Vec2);
    void opt_VecAddVecV256(V256 Vec1, V256 Vec2, V256 *Vec);

    void opt_MatMulVecM256(M256 Mat, V256 Vec, V256 *ans);
    void opt_MatMulMatM256(M256 Mat1, M256 Mat2, M256 *Mat);
    void opt_MattransM256(M256 Mat, M256 *Mat_trans);

    void opt_MatAddMatM256(M256 Mat1, M256 Mat2, M256 *Mat);
    void opt_genMatpairM256(M256 *Mat, M256 *Mat_inv);
    void opt_genaffinepairM256(Aff256 *aff, Aff256 *aff_inv);
    void opt_affinemixM256(Aff256 aff, Aff256 preaff_inv, Aff256 *mixaff);

#ifdef __cplusplus
}
#endif

#endif