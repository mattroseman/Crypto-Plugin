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
void rsa_decrypt_message(unsigned char*, unsigned int, RSA*, unsigned char**);
unsigned int aes_encrypt_message(unsigned char*, unsigned int, unsigned char*, unsigned char**, unsigned char**);
unsigned int aes_decrypt_message(unsigned char*, unsigned int, unsigned char*, unsigned char**, unsigned char**);

//  RSA constants
const int padding = RSA_PKCS1_PADDING;
const int kBits = 2048;
const unsigned long kExp_long = 65537;
BIGNUM *kExp = NULL;

//  AES constants
const int aes_key_length = 256;


int main() {
    RSA *key = generate_rsa_keys();

    printf ("RSA keys generated\n");

    unsigned char *plaintext = "Hello World!";
    unsigned int plaintext_length = 12;

    unsigned char *encrypted_message = (unsigned char *)malloc(RSA_size(key));

    printf ("beginning rsa encryption\n");

    unsigned int encrypted_length = rsa_encrypt_message(plaintext, plaintext_length, key, &encrypted_message);

    printf ("\'Hello World!\' encrypted is:\n");
    printf ("%s\n", encrypted_message);
    printf ("%u\n", encrypted_length);

    unsigned char *decrypted_message = (unsigned char *)malloc(plaintext_length*sizeof(unsigned char));

    rsa_decrypt_message(encrypted_message, encrypted_length, key, &decrypted_message);

    printf("The decrypted message is:\n");
    printf("%s\n", decrypted_message);
}


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

/*
 * Encrypts a plaintext and sets it to out
 * @param message: the plaintext string
 * @param length: the length of the plaintext string in bytes
 * @param key: the RSA object that is the key
 * @param out: where the output encrypted message is returned
 * @return: the length of the encrypted message
 */
unsigned int rsa_encrypt_message(unsigned char *message, unsigned int length, RSA *key, unsigned char **out) {
    unsigned int size;

    if ((size = RSA_public_encrypt(length, message, *out, key, padding)) < 0) {
        printf("RSA_public_encrypt() failed\n");
        exit(EXIT_FAILURE);
    }

    return size;
}

/* 
 * Decyrpts an encrypted text and sets it to out
 * @param encrypted_message: the RSA encrypted ciphertext
 * @param length: the length of the original plaintext
 * @param key: the RSA key to decrypt with
 * @param out: the string to print plaintext to
 */
void rsa_decrypt_message(unsigned char *encrypted_message, unsigned int length, RSA *key, unsigned char **out) {
    unsigned char *decrypted_message = (unsigned char *)malloc(length*sizeof(unsigned char));

    if (RSA_private_decrypt(length, encrypted_message, *out, key, padding) < 0) {
        printf("RSA_private_decrypt() failed\n");
        exit(EXIT_FAILURE);
    }
}
