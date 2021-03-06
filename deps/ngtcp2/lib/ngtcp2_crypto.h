/*
 * ngtcp2
 *
 * Copyright (c) 2017 ngtcp2 contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef NGTCP2_CRYPTO_H
#define NGTCP2_CRYPTO_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <ngtcp2/ngtcp2.h>

#include "ngtcp2_mem.h"

/* NGTCP2_INITIAL_AEAD_OVERHEAD is an overhead of AEAD used by Initial
   packets.  Because QUIC uses AEAD_AES_128_GCM, the overhead is 16
   bytes. */
#define NGTCP2_INITIAL_AEAD_OVERHEAD 16

/* NGTCP2_MAX_AEAD_OVERHEAD is expected maximum AEAD overhead. */
#define NGTCP2_MAX_AEAD_OVERHEAD 16

typedef enum {
  NGTCP2_CRYPTO_KM_FLAG_NONE,
  /* NGTCP2_CRYPTO_KM_FLAG_KEY_PHASE_ONE is set if key phase bit is
     set. */
  NGTCP2_CRYPTO_KM_FLAG_KEY_PHASE_ONE = 0x01,
} ngtcp2_crypto_km_flag;

typedef struct {
  ngtcp2_vec key;
  ngtcp2_vec iv;
  /* pkt_num is a packet number of a packet which uses this keying
     material.  For encryption key, it is the lowest packet number of
     a packet.  For decryption key, it is the lowest packet number of
     a packet which can be decrypted with this keying material. */
  int64_t pkt_num;
  /* flags is the bitwise OR of zero or more of
     ngtcp2_crypto_km_flag. */
  uint8_t flags;
} ngtcp2_crypto_km;

/*
 * ngtcp2_crypto_km_new creates new ngtcp2_crypto_km object and
 * assigns its pointer to |*pckm|.  The |key| of length |keylen| and
 * the |iv| of length |ivlen| are copied to |*pckm|.
 */
int ngtcp2_crypto_km_new(ngtcp2_crypto_km **pckm, const uint8_t *key,
                         size_t keylen, const uint8_t *iv, size_t ivlen,
                         const ngtcp2_mem *mem);

/*
 * ngtcp2_crypto_km_nocopy_new is similar to ngtcp2_crypto_km_new, but
 * it does not copy key and IV.
 */
int ngtcp2_crypto_km_nocopy_new(ngtcp2_crypto_km **pckm, size_t keylen,
                                size_t ivlen, const ngtcp2_mem *mem);

void ngtcp2_crypto_km_del(ngtcp2_crypto_km *ckm, const ngtcp2_mem *mem);

typedef struct {
  ngtcp2_crypto_aead aead;
  ngtcp2_crypto_cipher hp;
  const ngtcp2_crypto_km *ckm;
  const ngtcp2_vec *hp_key;
  size_t aead_overhead;
  ngtcp2_encrypt encrypt;
  ngtcp2_decrypt decrypt;
  ngtcp2_hp_mask hp_mask;
  void *user_data;
} ngtcp2_crypto_cc;

void ngtcp2_crypto_create_nonce(uint8_t *dest, const uint8_t *iv, size_t ivlen,
                                int64_t pkt_num);

#endif /* NGTCP2_CRYPTO_H */
