/* openssl headers */
#include "include/openssl/rsa.h"
#include "include/openssl/bn.h"
#include "include/openssl/evp.h"
#include "include/openssl/aes.h"
#include "include/openssl/rand.h"
#include "include/openssl/conf.h"
#include "include/openssl/evp.h"
#include "include/openssl/err.h"
#include "include/openssl/pem.h"

/* openssl base64 encypt/decrypt */
#include "OpenSSL_B64/Base64Encode.c"
#include "OpenSSL_B64/Base64Decode.c"

/* general headers */
#include <string.h>

RSA *generate_rsa_keys();
unsigned int rsa_publickey_to_pem(RSA*, unsigned char**);
RSA *rsa_pem_to_publickey(unsigned char*);
unsigned int rsa_privatekey_to_pem(RSA*, unsigned char**);
RSA *rsa_pem_to_privatekey(unsigned char*, RSA*);
unsigned char *generate_aes_key();
unsigned char *generate_aes_iv();
unsigned int rsa_encrypt_message(unsigned char*, unsigned int, RSA*, unsigned char**);
unsigned int rsa_decrypt_message(unsigned char*, unsigned int, RSA*, unsigned char**);
unsigned int aes_encrypt_message(unsigned char*, unsigned int, unsigned char*, unsigned char*, unsigned char**);
unsigned int aes_decrypt_message(unsigned char*, unsigned int, unsigned char*, unsigned char*, unsigned char**);

//  RSA constants
const int padding = RSA_PKCS1_PADDING;
const int kBits = 2048;
const int key_length = 2048/8;
const unsigned long kExp_long = 65537;
BIGNUM *kExp = NULL;
unsigned char *password = "P@ssw0rd";


//  AES constants
const int aes_key_length = 256;


int main() {
    RSA *rsa_key = generate_rsa_keys();

    unsigned char *rsa_pubkey_pem; 
    unsigned int pub_pem_size = rsa_publickey_to_pem(rsa_key, &rsa_pubkey_pem);

    printf ("the rsa public key generated in pem format\n");
    printf ("this pem should be %u bytes\n\n", pub_pem_size);
    printf ("%s\n", rsa_pubkey_pem);

    unsigned char *rsa_privkey_pem;
    unsigned int priv_pem_size = rsa_privatekey_to_pem(rsa_key, &rsa_privkey_pem);

    printf ("the rsa private key generated in pem format\n");
    printf ("this pem should be %u bytes\n\n", priv_pem_size);
    printf ("%s\n", rsa_privkey_pem);

    printf ("RSA keys generated\n");

    unsigned char *plaintext = (unsigned char *)"Hello World!";
    unsigned int plaintext_length = 12;

    unsigned char *encrypted_message = (unsigned char *)malloc(kBits);

    printf ("beginning rsa encryption\n");

    RSA *rsa_pubkey = rsa_pem_to_publickey(rsa_pubkey_pem);

    unsigned int encrypted_length = rsa_encrypt_message(plaintext, plaintext_length, rsa_pubkey, &encrypted_message);

    printf ("\'Hello World!\' encrypted is:\n");
    printf ("%s\n", encrypted_message);
    printf ("%u\n", encrypted_length);

    unsigned char *decrypted_message = (unsigned char *)malloc(plaintext_length*sizeof(unsigned char));

    printf ("beginning rsa decryption\n");

    RSA *rsa_privkey = rsa_pem_to_privatekey(rsa_privkey_pem, rsa_pubkey);

    unsigned char *temp_pub_pem;
    unsigned char *temp_priv_pem;
    unsigned int temp_pub_pem_size = rsa_publickey_to_pem(rsa_pubkey, &temp_pub_pem);
    unsigned int temp_priv_pem_size = rsa_privatekey_to_pem(rsa_privkey, &temp_priv_pem);

    printf("re encoded keys\n");
    printf("%s\n", temp_pub_pem);
    printf("%s\n", temp_priv_pem);
        

    rsa_decrypt_message(encrypted_message, encrypted_length, rsa_privkey, &decrypted_message);

    printf ("The decrypted message is:\n");
    printf ("%s\n", decrypted_message);

    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);

    unsigned char *aes_key = generate_aes_key();
    unsigned char *aes_iv = generate_aes_iv();

    printf ("AES key and iv generated\n");
    printf ("key: %s\n", aes_key);
    printf ("iv: %s\n", aes_iv);

    unsigned char *aes_encrypted_message = (unsigned char *)malloc(plaintext_length*(unsigned char) + AES_BLOCK_SIZE);

    printf ("beginning aes encryption\n");

    unsigned int aes_encrypted_length = aes_encrypt_message(plaintext, plaintext_length, aes_key, aes_iv, &aes_encrypted_message);

    printf ("\'Hello World encrypted is:\n");
    printf ("%s\n", aes_encrypted_message);
    printf ("%u\n", aes_encrypted_length);

    unsigned char *aes_decrypted_message = (unsigned char *)malloc(plaintext_length*(unsigned char) + AES_BLOCK_SIZE);

    unsigned int aes_decrypted_length = aes_decrypt_message(aes_encrypted_message, aes_encrypted_length, aes_key, aes_iv, &aes_decrypted_message);

    printf ("The decrypted message is:\n");
    printf ("%s\n", aes_decrypted_message);

    exit(EXIT_SUCCESS);
}


/*
 * Generates RSA public and private key
 * @param public_key: where the DER encoded public key is put
 * @param private_key: where the DER encoded private key is put
 * @return: pointer to RSA object
 */
RSA *generate_rsa_keys(unsigned char **public_key, unsigned char **private_key) {
    RSA *key = RSA_new();
    kExp = BN_new();
    BN_set_word(kExp, kExp_long);

    if (RSA_generate_key_ex(key, kBits, kExp, 0) < 0) {
        printf("RSA_generate_key_ex failed\n");
        exit(EXIT_FAILURE);
    }

    return key;
}

/* 
 * Takes in an RSA object and PEM encodes it in out
 * @param key: the RSA key
 * @param out: the string the PEM encoding goes to
 * @return: the length of the PEM encoding
 */
unsigned int rsa_publickey_to_pem(RSA *key, unsigned char **out) {
    //  This creats a source sink BIO to store the public key
    BIO *pubKey = BIO_new(BIO_s_mem());

    //  this writes the RSA key into the BIO in PEM format
    PEM_write_bio_RSA_PUBKEY(pubKey, key);

    unsigned char line[100];
    int len = 0;
    unsigned char *pem = NULL;
    unsigned char *new_pem = NULL;

    //  Have to do the first line because the pubkey BIO seems to get junk bytes at the beginning
    //  don't know why, but this is an easy fix
    if (!BIO_eof(pubKey)) {
        //  reads one line up to pubKey bytes and puts it into line
        if(!BIO_gets(pubKey, line, sizeof *pubKey)) {
            printf("BIO_gets for reading the public key pem to a string failed\n");
            exit(EXIT_FAILURE);
        }

        //  current length of PEM (including newlines)
        len += strlen(line);

        //  initialize the first allocation of memory for pem and new_pem
        if (!(new_pem = (unsigned char *)calloc(len, sizeof(unsigned char)))) {
            printf("calloc failed at length:%d\n", len);
            exit(EXIT_FAILURE);
        }

        memcpy(new_pem, line, (size_t)len);
        pem = new_pem;

        //  These lines are for debugging, show line lengths
        /*
        printf ("%s", line);
        printf ("length of line: %u\n", strlen(line));
        printf ("total length: %u %u\n", len, strlen(pem));
        */
    }

    while (!BIO_eof(pubKey)) {
        //  reads one line up to pubKey bytes and puts it into line
        if (!BIO_gets(pubKey, line, sizeof *pubKey)) {
            printf("BIO_gets for reading the public key pem to a string failed\n");
            exit(EXIT_FAILURE);
        }

        //  current length of PEM (including newlines)
        len += strlen(line);

        //  copy the data of pem over to new_pem but with the increased length for the new line
        if (!(new_pem = (unsigned char *)realloc(pem, len*sizeof(unsigned char)))) {
            printf("realloc failed at length:%d\n", len);
            exit(EXIT_FAILURE);
        }

        memcpy(new_pem, strcat(new_pem, line), (size_t)len);
        pem = new_pem;

        //  These lines are for debugging, show line lengths
        /*
        printf ("%s", line);
        printf ("length of line: %u\n", strlen(line));
        printf ("total length: %u %u\n", len, strlen(pem));
        */
    }

    *out = pem;

    return len;
}

/* 
 * Takes in an RSA object and PEM encodes it in out
 * @param key: the RSA private key
 * @param out: the string the PEM encoding goes to
 * @return: the length of the PEM encoding
 */
unsigned int rsa_privatekey_to_pem(RSA *key, unsigned char **out) {
    //  This creats a source sink BIO to store the private key
    BIO *privKey = BIO_new(BIO_s_mem());

    //  this writes the RSA key into the BIO in PEM format
    PEM_write_bio_RSAPrivateKey(privKey, key, NULL, NULL, 0, 0, NULL);

    unsigned char line[100];
    int len = 0;
    unsigned char *pem = NULL;
    unsigned char *new_pem = NULL;

    //  Have to do the first line because the pubkey BIO seems to get junk bytes at the beginning
    //  don't know why, but this is an easy fix
    if (!BIO_eof(privKey)) {
        //  reads one line up to pubKey bytes and puts it into line
        if (!BIO_gets(privKey, line, sizeof *privKey)) {
            printf("BIO_gets for reading the private key pem to a string failed\n");
            exit(EXIT_FAILURE);
        }

        //  current length of PEM (including newlines)
        len += strlen(line);

        //  create the first allocation of data, and fill all data with 0's
        if (!(new_pem = (unsigned char *)calloc(len, sizeof(unsigned char)))) {
            printf("calloc failed at length:%d\n", len);
            exit(EXIT_FAILURE);
        } 

        //memcpy(new_pem, "-----BEGIN RSA PRIVATE KEY-----\n", (size_t)len);
        memcpy(new_pem, line, (size_t)len);
        pem = new_pem;

        //  These lines are for debugging, show line lengths
        /*
        printf ("%s", line);
        printf ("length of line: %u\n", strlen(line));
        printf ("total length: %u %u\n", len, strlen(pem));
        */
    }

    while (!BIO_eof(privKey)) {
        //  reads one line up to pubKey bytes and puts it into line
        if (!BIO_gets(privKey, line, sizeof *privKey)) {
            printf("BIO_gets for reading the private key pem to a string failed\n");
            exit(EXIT_FAILURE);
        }

        //  current length of PEM (including newlines)
        len += strlen(line);

        //  copy the data of pem over to new_pem but with the increased length for the new line
        if (!(new_pem = (unsigned char *)realloc(pem, (len + 1)*sizeof(unsigned char)))) {
            printf("realloc failed at length:%d\n", len);
            exit(EXIT_FAILURE);
        }

        memcpy(new_pem, strcat(new_pem, line), (size_t)len);
        pem = new_pem;

        //  These lines are for debugging, show line lengths
        /*
        printf ("%s", line);
        printf ("length of line: %u\n", strlen(line));
        printf ("total length: %u %u\n", len, strlen(pem));
        */
    }

    *out = pem;

    return len;
}

/* 
 * Takes a PEM encoding of a public rsa key and converts it to an RSA object
 * @param public_key: the PEM encoding
 * @return: the RSA public key (no private key)
 */
RSA *rsa_pem_to_publickey(unsigned char *public_key) {
    RSA *key = RSA_new();
    BIO *pubKey = BIO_new(BIO_s_mem());
    int len;

    if ((len = BIO_puts(pubKey, public_key)) <= 0) {
        printf ("BIO_puts in set_rsa_public_key failed\n");
        exit(EXIT_FAILURE);
    }
    
    PEM_read_bio_RSA_PUBKEY(pubKey, &key, NULL, NULL);

    return key;
}

/* 
 * Takes a PEM encoding of a private rsa key and converts it to an RSA object
 * @param private_key: the PEM encoding
 * @return: the RSA private key
 */
RSA *rsa_pem_to_privatekey(unsigned char *private_key,  RSA *public_key) {
    /*
    if (!EVP_PKEY_assign_RSA(key, empty_key)) {
        printf("EVP_PKEY_set1_RSA failed\n");
        exit(EXIT_FAILURE);
    }
    */

    BIO *privKey = BIO_new(BIO_s_mem());
    int len;

    if ((len = BIO_puts(privKey, private_key)) <= 0) {
        printf ("BIO_puts in set_rsa_public_key failed\n");
        exit(EXIT_FAILURE);
    }

    EVP_PKEY *key = PEM_read_bio_PrivateKey(privKey, NULL, NULL, NULL);

    RSA *rsa_key = RSA_new();

    if (!(rsa_key = EVP_PKEY_get1_RSA(key))) {
        printf("EVP_PKEY_get1_RSA failed\n");
        exit(EXIT_FAILURE);
    }

    return rsa_key;
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
    unsigned char *encrypted_message = (unsigned char *)malloc(RSA_size(key));
    int size;

    if ((size = RSA_public_encrypt(length, message, encrypted_message, key, padding)) < 0) {
        printf("RSA_public_encrypt() failed\n");
        exit(EXIT_FAILURE);
    }

    if (Base64Encode(encrypted_message, size, (char **)out) < 0) {
        printf("Base64Encode failed\n");
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
unsigned int rsa_decrypt_message(unsigned char *encrypted_message, unsigned int length, RSA *key, unsigned char **out) {
    unsigned char *encrypted_ascii_message = (unsigned char *)malloc(key_length);
    unsigned char *decrypted_message = (unsigned char *)malloc(length);
    unsigned int size;
    
    if (Base64Decode((char *)encrypted_message, &encrypted_ascii_message, (size_t *)&length) < 0) {
        printf("Base64Decode failed\n");
        exit(EXIT_FAILURE);
    }

    printf ("message decoded\n");

    if ((size = RSA_private_decrypt(length, encrypted_ascii_message, decrypted_message, key, padding)) < 0) {
        printf("RSA_private_decrypt() failed\n");
        exit(EXIT_FAILURE);
    }

    printf ("message decrypted\n");

    *(decrypted_message + size) = '\0';

    *out = decrypted_message;

    return size;
}

/* 
 * Creates the aes key which is essentially 256 random bits
 * @return: the string representation of the aes key
 */
unsigned char *generate_aes_key() {
    unsigned char *encKey = (unsigned char *)malloc(aes_key_length);

    if (!RAND_bytes(encKey, sizeof encKey)) {
        printf("RAND_bytes for encKey failed\n");
        exit(EXIT_FAILURE);
    }

    return encKey;
}

/* 
 * Creates the aes iv which is essentially 128 random bits
 * @return: the string representation of the aes iv
 */
unsigned char *generate_aes_iv() {
    unsigned char *encIv = (unsigned char *)malloc(AES_BLOCK_SIZE*sizeof(unsigned char));

    if (!RAND_bytes(encIv, sizeof encIv)) {
        printf("RAND_bytes for encIv failed\n");
        exit(EXIT_FAILURE);
    }

    return encIv;
}

/* 
 * Encrypts the plaintext and sets it to out
 * @param message: the plaintext string
 * @param length: the number of chars for the plaintext
 * @param encKey: the aes 256 key
 * @param encIv: the aes 128 iv
 * @param out: where the enc message is put
 */
unsigned int aes_encrypt_message(unsigned char *message, unsigned int length, unsigned char *encKey, unsigned char *encIv, unsigned char **out) {
    unsigned char *encMsg = (unsigned char *)malloc(length*(unsigned char) + AES_BLOCK_SIZE);
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    if (!(ctx = EVP_CIPHER_CTX_new())) {
        printf ("EVP_CIPHER_CTX_new() failed\n");
        exit (EXIT_FAILURE);
    }

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, encKey, encIv)) {
        printf ("EVP_EncryptInit_ex failed\n");
        exit (EXIT_FAILURE);
    }

    if (1 != EVP_EncryptUpdate(ctx, encMsg, &len, message, length)) {
        printf ("EVP_ENcryptUpdate failed\n");
        exit (EXIT_FAILURE);
    }

    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, encMsg + len, &len)) {
        printf ("EVP_EncryptFinal_ex() failed\n");
        exit (EXIT_FAILURE);
    }

    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    *(encMsg + ciphertext_len) = '\0';

    if (Base64Encode(encMsg, ciphertext_len, (char **)out) < 0) {
        printf ("Base64Encode in aes_encrypt_message failed\n");
    }

    return ciphertext_len;
}

/* 
 * Decrypts the aes encrypted text and sets it to out
 */
unsigned int aes_decrypt_message(unsigned char *encrypted_message, unsigned int length, unsigned char *encKey, unsigned char *encIv, unsigned char **out) {
    unsigned char *encrypted_ascii_message = (unsigned char *)malloc(length*(unsigned char) + AES_BLOCK_SIZE);

    if (Base64Decode((char *)encrypted_message, &encrypted_ascii_message, (size_t *)&length) < 0) {
        printf("Base64Decode in aes_decrypt_message failed\n");
        exit(EXIT_FAILURE);
    }

    EVP_CIPHER_CTX *ctx;
    unsigned char *decMsg = (unsigned char *)malloc(length*(unsigned char) + AES_BLOCK_SIZE);

    int plaintext_len;
    int len;

    if (!(ctx = EVP_CIPHER_CTX_new())) {
        printf("EVP_CIPHER_CTX_new() failed\n");
        exit(EXIT_FAILURE);
    }

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, encKey, encIv)) {
        printf("EVP_DecryptInit_ex() failed\n");
        exit(EXIT_FAILURE);
    }

    if (1 != EVP_DecryptUpdate(ctx, decMsg, &len, encrypted_ascii_message, length)) {
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

    *out = decMsg;

    return plaintext_len;
}
