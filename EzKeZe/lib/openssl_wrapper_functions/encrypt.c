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

RSA* generate_rsa_keys();
AES_KEY *generate_aes_key();
unsigned int rsa_encrypt_message(unsigned char*, unsigned int, RSA*, unsigned char**);
unsigned int rsa_decrypt_message(unsigned char*, unsigned int, RSA*, unsigned char**);
unsigned int aes_encrypt_message(unsigned char*, unsigned int, AES_KEY*, unsigned char**, unsigned char**);
void aes_decrypt_message(unsigned char*, unsigned int, AES_KEY*, unsigned char*, unsigned char**);
void print_rsa_keys(RSA*, FILE*);

int padding = RSA_PKCS1_PADDING;

const int kBits = 2048; //  the length of the private key
const unsigned long kExp_long = 65537;

const int aes_key_length = 256;

BIGNUM *kExp = NULL;

RSA *rsa; //  the RSA keys

char *rsa_key_filename = "rsa_key.txt"; //  the file to print the RSA keys to
char *rsa_encrypted_filename = "encrypted_text.txt";

unsigned char *plaintext = (unsigned char *)"Hello World!";
unsigned int message_length = 12;


int main() {
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);


//#BEGIN GENERATE RSA KEYS
    kExp = BN_new();
    BN_set_word(kExp, kExp_long);

    rsa = generate_rsa_keys();

    //  create the file to print the rsa key to
    FILE *rsa_key_file = fopen(rsa_key_filename, "w+"); 
    print_rsa_keys(rsa, rsa_key_file);
//#END
//#BEGIN RSA ENCRYPTION
    unsigned char *encrypted_message = (unsigned char *)malloc(RSA_size(rsa));
    message_length = rsa_encrypt_message(plaintext, message_length, rsa, &encrypted_message);

    printf("\"%s\" has been encrypted to:\n", plaintext);
    printf("%s\n", encrypted_message);
//#END
//#BEGIN DECRYPTION
    unsigned char *unencrypted_message = (unsigned char *)malloc(RSA_size(rsa));
  
    message_length = rsa_decrypt_message(encrypted_message, message_length, rsa, &unencrypted_message);

    printf("the unencrypted message is:\n");
    printf("%s\n", unencrypted_message);

    printf("%u\n", message_length);
//#END
//#BEGIN GENERATE AES KEY
    //AES_KEY *aes_key = generate_aes_key();
//#END

    unsigned char *encMsg = (unsigned char *)malloc(message_length*(unsigned char) + AES_BLOCK_SIZE);
    unsigned char *decMsg = (unsigned char *)malloc(message_length*(unsigned char) + AES_BLOCK_SIZE);
    unsigned char *encMsg_buffer = (unsigned char *)malloc(128*sizeof(unsigned char));
    unsigned char *decMsg_buffer = (unsigned char *)malloc(128*sizeof(unsigned char));
    unsigned char *encIv = (unsigned char *)malloc(AES_BLOCK_SIZE*sizeof(unsigned char));
    unsigned char *encKey = (unsigned char *)malloc(aes_key_length);

    if (!RAND_bytes(encKey, sizeof encKey)) {
        printf("RAND_bytes for key failed\n");
        exit(EXIT_FAILURE);
    }

    if (!RAND_bytes(encIv, sizeof encIv)) {
        printf("RAND_bytes for iv failed\n");
        exit(EXIT_FAILURE);
    }

    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    if (!(ctx = EVP_CIPHER_CTX_new())) {
        printf("EVP_CIPHER_CTX_new() failed\n");
        exit(EXIT_FAILURE);
    }

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, encKey, encIv)) {
        printf("EVP_EncryptInit_ex failed\n");
        exit(EXIT_FAILURE);
    }

    if (1 != EVP_EncryptUpdate(ctx, encMsg, &len, plaintext, message_length)) {
        printf("EVP_ENcryptUpdate failed\n");
        exit(EXIT_FAILURE);
    }

    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, encMsg + len, &len)) {
        printf("EVP_EncryptFinal_ex() failed\n");
        exit(EXIT_FAILURE);
    }

    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    *(encMsg + ciphertext_len) = '\0';

    printf("AES encrypted message:\n");
    printf("%s\n", encMsg);

    int plaintext_len;

    if (!(ctx = EVP_CIPHER_CTX_new())) {
        printf("EVP_CIPHER_CTX_new() failed\n");
        exit(EXIT_FAILURE);
    }

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, encKey, encIv)) {
        printf("EVP_DecryptInit_ex() failed\n");
        exit(EXIT_FAILURE);
    }

    if (1 != EVP_DecryptUpdate(ctx, decMsg, &len, encMsg, ciphertext_len)) {
        printf("EVP_DecryptUpdate() failed\n");
        exit(EXIT_FAILURE);
    }

    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, decMsg + len, &len)) {
        printf("EVP_DecryptFinal_ex() failed\n");
        exit(EXIT_FAILURE);
    }

    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    *(decMsg + plaintext_len) = '\0';

    printf("AES decrypted message:\n");
    printf("%s\n", decMsg);

    EVP_cleanup();
    ERR_free_strings();


    exit(EXIT_SUCCESS);
}

/*
 * Generates some RSA keys
 * @return: RSA* object
 */
RSA* generate_rsa_keys() {
    //  generate key pair and store in rsa
    RSA *rsa_temp = RSA_new();

    if (RSA_generate_key_ex(rsa_temp, kBits, kExp, 0) < 0)
    {
        printf("RSA_generate_key_ex failed\n");
        exit(EXIT_FAILURE);
    }

    //  XXX this is unused now but may be used later
    /*
    unsigned char* e_b = (unsigned char*)calloc(RSA_size(rsa_temp), sizeof(unsigned char));
    //  converts public key to binary
    int e_size = BN_bn2bin(rsa_temp->e, e_b);
    */

    return rsa_temp;
}

/*
 * Generate the AES symmetric key
 * @return: base 64 encoded aes key
 */
AES_KEY *generate_aes_key() {
    AES_KEY *key = (AES_KEY *)malloc(sizeof(AES_KEY));;
    unsigned char key_text[aes_key_length/8];

    //  fill the memeory with 0 bytes
    memset(key_text, 0, aes_key_length/8);
    //  generates the actual aes key
    if (!RAND_bytes(key_text, sizeof key)) {
        printf("RAND_bytes for aes_key failed\n");
        exit(EXIT_FAILURE);
    }

    AES_set_encrypt_key(key_text, 256, key);

    /*
    //  generates the iv to distinguish multiple encryptions
    if (!RAND_bytes(iv, sizeof iv)) {
        printf("RAND_bytes for iv failed\n");
        exit(EXIT_FAILURE);
    }
    */

    return key;
}

/* 
 * Encrypts a plaintext message with an RSA key
 * @param message: the plaintext message to encrypt
 * @param length: the length in bytes of the message (or characters)
 * @param key: the RSA key used to encrypt the message
 * @param out: the string to print the encrypted message to
 * @return: the length in bytes of the encypted data
 */
unsigned int rsa_encrypt_message(unsigned char* message, unsigned int length, RSA* key, unsigned char** out) {
    unsigned char *encrypted_message = (unsigned char*)malloc(RSA_size(rsa));
    unsigned int size;

    if ((size = RSA_public_encrypt(length, message, encrypted_message, key, padding)) < 0) {;
        printf("RSA_public_encrypt() failed\n");
        exit(EXIT_FAILURE);
    }

    if ((size = Base64Encode(encrypted_message, RSA_size(rsa), (char **)out)) < 0) {
        printf("Base64Encode failed\n");
        exit(EXIT_FAILURE);
    }

    return size;
}

/* 
 * Encrypts a plaintext message with an AES key
 */
unsigned int aes_encrypt_message(unsigned char* message, unsigned int length, AES_KEY* key, unsigned char** iv_out, unsigned char** enc_out) {
    //  generates the iv to distinguish multiple encryptions
    unsigned char *iv = (unsigned char *)malloc(AES_BLOCK_SIZE*sizeof(unsigned char));

    if (!RAND_bytes(iv, sizeof iv)) {
        printf("RAND_bytes for iv failed\n");
        exit(EXIT_FAILURE);
    }

    *iv_out = iv;

    //  encrypt the message using the iv and the key
    unsigned char *encrypted_message = (unsigned char *)malloc(aes_key_length*sizeof(unsigned char));

    AES_cbc_encrypt(message, encrypted_message, length, key, iv, AES_ENCRYPT);

    if (Base64Encode((char *)encrypted_message, (size_t)length, (char **)enc_out) < 0) {
        printf("Base64Encode for aes_encrypt failed\n");
        exit(EXIT_FAILURE);
    }

    return length;
}

/*
 * Decrypts a base 64 encoded rsa encrypted message with an RSA key
 * @param encrypted_message: the base 64 rsa encrypted string
 * @param length: the length of the message
 * @param key: the RSA key object
 * @param out: the string the plaintext message is printed to
 * @return: the length of the plaintext message
 */
unsigned int rsa_decrypt_message(unsigned char* encrypted_message, unsigned int length, RSA* key, unsigned char** out) {
    unsigned char *encrypted_ascii_message = (unsigned char *)malloc(RSA_size(rsa));
    unsigned int size;

    if (Base64Decode((char *)encrypted_message, &encrypted_ascii_message, (size_t *)&length) < 0) {
        printf("Base64Decode failed\n");
        exit(EXIT_FAILURE);
    }

    if ((size = RSA_private_decrypt(length, encrypted_ascii_message, *out, rsa, padding)) < 0) {
        printf("RSA_public_decrypt() failed\n");
        exit(EXIT_FAILURE);
    }

    //  make the last character a terminating character
    *(*out + size) = '\0';

    return size;
}

void aes_decrypt_message(unsigned char* encrypted_message, unsigned int length, AES_KEY* key, unsigned char* iv, unsigned char** message) {
    unsigned char *encrypted_ascii_message = (unsigned char *) malloc(aes_key_length*sizeof(unsigned char));

    if (Base64Decode((char *)encrypted_message, &encrypted_ascii_message, (size_t *)&length) < 0) {
        printf("Base64Decode in aes decrypt failed\n");
        exit(EXIT_FAILURE);
    }

    AES_decrypt(encrypted_ascii_message, *message, key);
}

/*
 * Prints some RSA keys to a file
 * @param rsa: the keys to print
 * @param file: the file to print to
 */
void print_rsa_keys(RSA* rsa, FILE* file) {
    RSA_print_fp(file, rsa, 0);
}

