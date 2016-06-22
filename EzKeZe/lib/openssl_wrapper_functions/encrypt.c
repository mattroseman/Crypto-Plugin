/* openssl headers */
#include "include/openssl/rsa.h"
#include "include/openssl/bn.h"
#include "include/openssl/evp.h"
#include "include/openssl/aes.h"
#include "include/openssl/rand.h"
#include "include/openssl/conf.h"
#include "include/openssl/evp.h"
#include "include/openssl/err.h"

/* openssl base64 encypt/decrypt */
#include "OpenSSL_B64/Base64Encode.c"
#include "OpenSSL_B64/Base64Decode.c"

/* general headers */
#include <string.h>

RSA *generate_rsa_keys();
unsigned char *generate_aes_key();
unsigned int rsa_encrypt_message(unsigned char*, unsigned int, RSA*, unsigned char**);
unsigned int rsa_decrypt_message(unsigned char*, unsigned int, RSA*, unsigned char**);
unsigned int aes_encrypt_message(unsigned char*, unsigned int, unsigned char*, unsigned char**, unsigned char**);
unsigned int aes_decrypt_message(unsigned char*, unsigned int, unsigned char*, unsigned char**, unsigned char**);

//  RSA constants
const int padding = RSA_PKCS1_PADDING;
const int kBits = 2048;
const unsigned long kExp_long = 65537;
BIGNUM *kExp = NULL;

//  AES constants
const int aes_key_length = 256;


/*
 * Generates RSA public and private key
 * @return: pointer to RSA object
 */
RSA *generate_rsa_keys() {
    RSA *rsa = RSA_new();
    kExp = BN_new();
    BN_set_word(kExp, kExp_long);

    if (RSA_generate_key_ex(rsa, kBits, kExp, 0) < 0) {
        printf("RSA_generate_key_ex failed\n");
        exit(EXIT_FAILURE);
    }

    return rsa;
}
