/* openssl headers */
//#include "include/openssl/opensslconf.h"
#include "openssl/rsa.h"
#include "openssl/bn.h"
#include "openssl/evp.h"
#include "openssl/aes.h"
#include "openssl/rand.h"
#include "openssl/conf.h"
#include "openssl/evp.h"
#include "openssl/err.h"
#include "openssl/pem.h"

/* general headers */
#include <string.h>
#include <assert.h>

int Base64Encode(const unsigned char*, size_t, char**); //Encodes a binary safe base 64 string
size_t calcDecodeLength(const char*); //Calculates the length of a decoded string
int Base64Decode(char*, unsigned char**, size_t*); //Decodes a base64 encoded string

RSA *generate_rsa_keys();
unsigned int rsa_publickey_to_pem(RSA*, unsigned char**);
RSA *rsa_pem_to_publickey(unsigned char*);
unsigned int rsa_privatekey_to_pem(RSA*, unsigned char**, unsigned char*);
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


//  AES constants
const int aes_key_length = 256;


int main() {
    RSA *rsa_key = generate_rsa_keys();

    unsigned char *rsa_pubkey_pem; 
    unsigned int pub_pem_size = rsa_publickey_to_pem(rsa_key, &rsa_pubkey_pem);

    printf ("%s\n", rsa_pubkey_pem);

    unsigned char *rsa_privkey_pem;
    unsigned int priv_pem_size = rsa_privatekey_to_pem(rsa_key, &rsa_privkey_pem, NULL);

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

    printf ("%d\n", encrypted_length);

    unsigned char *decrypted_message = (unsigned char *)malloc(plaintext_length*sizeof(unsigned char));

    printf ("beginning rsa decryption\n");

    RSA *rsa_privkey = rsa_pem_to_privatekey(rsa_privkey_pem, rsa_key);

    rsa_decrypt_message(encrypted_message, encrypted_length, rsa_privkey, &decrypted_message);

    printf ("The decrypted message is:\n");
    printf ("%s\n", decrypted_message);

    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);

    unsigned char *aes_key = generate_aes_key();
    unsigned char *aes_iv = generate_aes_iv();

    printf ("AES key and iv generated\n");

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
    BIO *pubKey = BIO_new(BIO_s_mem());

    PEM_write_bio_RSA_PUBKEY(pubKey, key);

    unsigned char line[65];
    int len = 0;
    unsigned char *pem = NULL;
    unsigned char *new_pem = NULL;

    if (!BIO_eof(pubKey)) {
        BIO_gets(pubKey, line, sizeof *pubKey);

        len += strlen(line);

        new_pem = (unsigned char *)realloc(pem, len*sizeof(unsigned char));
        if (!new_pem) {
            printf("realloc failed at length:%d\n", len);
        } else {
            memcpy(new_pem, "-----BEGIN PUBLIC KEY-----\n", (size_t)len);
            pem = new_pem;
        }
    }

    while (!BIO_eof(pubKey)) {
        BIO_gets(pubKey, line, sizeof *pubKey);

        //  current length of PEM (including newlines)
        len += strlen(line);

        new_pem = (unsigned char *)realloc(pem, len*sizeof(unsigned char));
        if (!new_pem) {
            printf("realloc failed at length:%d\n", len);
            exit(EXIT_FAILURE);
        } else {
            memcpy(new_pem, strcat(new_pem, line), (size_t)len);
            pem = new_pem;
        }
    }

    *out = pem;

    return len;
}

/* 
 * Takes in an RSA object and PEM encodes it in out
 * @param key: the RSA private key
 * @param out: the string the PEM encoding goes to
 * @param pem_password: the password to unlock the pem encoding
 * @return: the length of the PEM encoding
 */
unsigned int rsa_privatekey_to_pem(RSA *key, unsigned char **out, unsigned char *password) {
    BIO *pubKey = BIO_new(BIO_s_mem());

    PEM_write_bio_RSAPrivateKey(pubKey, key, NULL, NULL, 0, NULL, NULL);

    unsigned char line[65];
    int len = 0;
    unsigned char *pem = NULL;
    unsigned char *new_pem = NULL;

    if (!BIO_eof(pubKey)) {
        BIO_gets(pubKey, line, sizeof *pubKey);

        len += strlen(line);

        new_pem = (unsigned char *)realloc(pem, len*sizeof(unsigned char));
        if (!new_pem) {
            printf("realloc failed at length:%d\n", len);
        } else {
            memcpy(new_pem, "-----BEGIN PRIVATE KEY-----\n", (size_t)len);
            pem = new_pem;
        }
    }

    while (!BIO_eof(pubKey)) {
        BIO_gets(pubKey, line, sizeof *pubKey);

        //  current length of PEM (including newlines)
        len += strlen(line);

        new_pem = (unsigned char *)realloc(pem, len*sizeof(unsigned char));
        if (!new_pem) {
            printf("realloc failed at length:%d\n", len);
            exit(EXIT_FAILURE);
        } else {
            memcpy(new_pem, strcat(new_pem, line), (size_t)len);
            pem = new_pem;
        }
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
RSA *rsa_pem_to_privatekey(unsigned char *private_key, RSA *public_key) {
    BIO *privKey = BIO_new(BIO_s_mem());
    int len;

    if ((len = BIO_puts(privKey, private_key)) <= 0) {
        printf ("BIO_puts in set_rsa_public_key failed\n");
        exit(EXIT_FAILURE);
    }
    
    PEM_read_bio_RSAPrivateKey(privKey, &public_key, NULL, NULL);

    return public_key;
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
    unsigned int size;

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

int Base64Encode(const unsigned char* buffer, size_t length, char** b64text) { //Encodes a binary safe base 64 string
	BIO *bio, *b64;
	BUF_MEM *bufferPtr;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
	BIO_write(bio, buffer, length);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &bufferPtr);
	BIO_set_close(bio, BIO_NOCLOSE);
	BIO_free_all(bio);

	*b64text=(*bufferPtr).data;

	return (0); //success
}

size_t calcDecodeLength(const char* b64input) { //Calculates the length of a decoded string
	size_t len = strlen(b64input),
		padding = 0;

	if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
		padding = 2;
	else if (b64input[len-1] == '=') //last char is =
		padding = 1;

	return (len*3)/4 - padding;
}

int Base64Decode(char* b64message, unsigned char** buffer, size_t* length) { //Decodes a base64 encoded string
	BIO *bio, *b64;

	int decodeLen = calcDecodeLength(b64message);
	*buffer = (unsigned char*)malloc(decodeLen + 1);
	(*buffer)[decodeLen] = '\0';

	bio = BIO_new_mem_buf(b64message, -1);
	b64 = BIO_new(BIO_f_base64());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer
	*length = BIO_read(bio, *buffer, strlen(b64message));
	assert(*length == decodeLen); //length should equal decodeLen, else something went horribly wrong
	BIO_free_all(bio);

	return (0); //success
}
