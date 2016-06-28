#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/var_dictionary.h"

#include "openssl/rsa.h"
#include "openssl/bn.h"
#include "openssl/evp.h"
#include "openssl/aes.h"
#include "openssl/rand.h"
#include "openssl/conf.h"
#include "openssl/evp.h"
#include "openssl/err.h"
#include "openssl/pem.h"
#include "openssl/bio.h"


int Base64Encode(const char*, size_t, char**); //Encodes a binary safe base 64 string
size_t calcDecodeLength(const char*); //Calculates the length of a decoded string
int Base64Decode(char*, char**, size_t*); //Decodes a base64 encoded string

RSA *generate_rsa_keys();
unsigned int rsa_publickey_to_pem(RSA*, char**);
RSA *rsa_pem_to_publickey(char*);
unsigned int rsa_privatekey_to_pem(RSA*, char**, char*);
RSA *rsa_pem_to_privatekey(char*, RSA*);
char *generate_aes_key();
char *generate_aes_iv();
unsigned int rsa_encrypt_message(char*, unsigned int, RSA*, char**);
unsigned int rsa_decrypt_message(char*, unsigned int, RSA*, char**);
unsigned int aes_encrypt_message(char*, unsigned int, char*, char*, char**);
unsigned int aes_decrypt_message(char*, unsigned int, char*, char*, char**);

//  RSA constants
const int padding = RSA_PKCS1_PADDING;
const int kBits = 2048;
const int key_length = 2048/8;
const unsigned long kExp_long = 65537;
BIGNUM *kExp = NULL;


//  AES constants
const int aes_key_length = 256;


class EncryptInstance : public pp::Instance {
    public:
        explicit EncryptInstance(PP_Instance instance) : pp::Instance(instance) {}
        virtual ~EncryptInstance() {}

        virtual void HandleMessage(const pp::Var& var_message) { 
            //  if the message isn't a string
            if (var_message.is_dictionary()) {
                pp::VarDictionary dictionary(var_message);
                pp::VarArray keys = dictionary.GetKeys();

                //  if asked to generate an rsa key
                if (dictionary.Get(pp::Var("request_type")).AsString().compare("generate_rsa_keys") == 0) {
                    RSA *rsa_key = generate_rsa_keys();
                    char *rsa_pubkey_pem;
                    char *rsa_privkey_pem;
                    rsa_publickey_to_pem(rsa_key, &rsa_pubkey_pem);
                }

                //  if asked to encrypt a string with a given rsa key
                if (dictionary.Get(pp::Var("request_type")).AsString().compare("encrypt_rsa") == 0) {
                }

                //  if asked to decrypt a string with a given rsa key
                if (dictionary.Get(pp::Var("request_type")).AsString().compare("decrypt_rsa") == 0) {
                }

                //  if asked to generate an aes key
                if (dictionary.Get(pp::Var("request_type")).AsString().compare("generate_aes_keys") == 0) {
                }

                //  if asked to encrypt a string with a given aes key
                if (dictionary.Get(pp::Var("request_type")).AsString().compare("encrypt_aes") == 0) {
                }

                //  if asked to decrypt a string with a given aes key
                if (dictionary.Get(pp::Var("request_type")).AsString().compare("decrypt_aes") == 0) {
                }

                pp::Var var_reply;
            }
        }
};

class EncryptModule : public pp::Module {
    public:
        EncryptModule() : pp::Module() {}
        virtual ~EncryptModule() {}

        virtual pp::Instance* CreateInstance(PP_Instance instance) {
            return new EncryptInstance(instance);
        }
};

namespace pp {
    Module* CreateModule() {
        return new EncryptModule();
    }
}


/*
 * Generates RSA public and private key
 * @param public_key: where the DER encoded public key is put
 * @param private_key: where the DER encoded private key is put
 * @return: pointer to RSA object
 */
RSA *generate_rsa_keys() {
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
unsigned int rsa_publickey_to_pem(RSA *key, char **out) {
    BIO *pubKey = BIO_new(BIO_s_mem());

    PEM_write_bio_RSA_PUBKEY(pubKey, key);

    char line[65];
    int len = 0;
    char *pem = NULL;
    char *new_pem = NULL;

    if (!BIO_eof(pubKey)) {
        BIO_gets(pubKey, line, sizeof *pubKey);

        len += strlen(line);

        new_pem = (char *)realloc(pem, len*sizeof(char));
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

        new_pem = (char *)realloc(pem, len*sizeof(char));
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
unsigned int rsa_privatekey_to_pem(RSA *key, char **out, char *password) {
    BIO *pubKey = BIO_new(BIO_s_mem());

    PEM_write_bio_RSAPrivateKey(pubKey, key, NULL, NULL, 0, NULL, NULL);

    char line[65];
    int len = 0;
    char *pem = NULL;
    char *new_pem = NULL;

    if (!BIO_eof(pubKey)) {
        BIO_gets(pubKey, line, sizeof *pubKey);

        len += strlen(line);

        new_pem = (char *)realloc(pem, len*sizeof(char));
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

        new_pem = (char *)realloc(pem, len*sizeof(char));
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
RSA *rsa_pem_to_publickey(char *public_key) {
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
RSA *rsa_pem_to_privatekey(char *private_key, RSA *public_key) {
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
int rsa_encrypt_message(unsigned char *message, unsigned int length, RSA *key, char **out) {
    char *encrypted_message = (char *)malloc(RSA_size(key));
    int size;

    if ((size = RSA_public_encrypt(length, message, (unsigned char *)encrypted_message, key, padding)) < 0) {
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
unsigned int rsa_decrypt_message(char *encrypted_message, unsigned int length, RSA *key, char **out) {
    char *encrypted_ascii_message = (char *)malloc(key_length);
    char *decrypted_message = (char *)malloc(length);
    int size;
    
    if (Base64Decode((char *)encrypted_message, &encrypted_ascii_message, (size_t *)&length) < 0) {
        printf("Base64Decode failed\n");
        exit(EXIT_FAILURE);
    }

    printf ("message decoded\n");

    if ((size = RSA_private_decrypt(length, (unsigned char *)encrypted_ascii_message, (unsigned char *)decrypted_message, key, padding)) < 0) {
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
char *generate_aes_key() {
    char *encKey = (char *)malloc(aes_key_length);

    if (!RAND_bytes((unsigned char *)encKey, sizeof encKey)) {
        printf("RAND_bytes for encKey failed\n");
        exit(EXIT_FAILURE);
    }

    return encKey;
}

/* 
 * Creates the aes iv which is essentially 128 random bits
 * @return: the string representation of the aes iv
 */
char *generate_aes_iv() {
    char *encIv = (char *)malloc(AES_BLOCK_SIZE*sizeof(char));

    if (!RAND_bytes((unsigned char *)encIv, sizeof encIv)) {
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
unsigned int aes_encrypt_message(char *message, unsigned int length, char *encKey, char *encIv, char **out) {
    char *encMsg = (char *)malloc(length*(char) + AES_BLOCK_SIZE);
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    if (!(ctx = EVP_CIPHER_CTX_new())) {
        printf ("EVP_CIPHER_CTX_new() failed\n");
        exit (EXIT_FAILURE);
    }

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned char *)encKey, (const unsigned char *)encIv)) {
        printf ("EVP_EncryptInit_ex failed\n");
        exit (EXIT_FAILURE);
    }

    if (1 != EVP_EncryptUpdate(ctx, (unsigned char *)encMsg, &len, (const unsigned char *)message, length)) {
        printf ("EVP_ENcryptUpdate failed\n");
        exit (EXIT_FAILURE);
    }

    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, (unsigned char *)(encMsg + len), &len)) {
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
unsigned int aes_decrypt_message(char *encrypted_message, unsigned int length, char *encKey, char *encIv, char **out) {
    char *encrypted_ascii_message = (char *)malloc(length*(char) + AES_BLOCK_SIZE);

    if (Base64Decode((char *)encrypted_message, &encrypted_ascii_message, (size_t *)&length) < 0) {
        printf("Base64Decode in aes_decrypt_message failed\n");
        exit(EXIT_FAILURE);
    }

    EVP_CIPHER_CTX *ctx;
    char *decMsg = (char *)malloc(length*(char) + AES_BLOCK_SIZE);

    int plaintext_len;
    int len;

    if (!(ctx = EVP_CIPHER_CTX_new())) {
        printf("EVP_CIPHER_CTX_new() failed\n");
        exit(EXIT_FAILURE);
    }

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned char *)encKey, (const unsigned char *)encIv)) {
        printf("EVP_DecryptInit_ex() failed\n");
        exit(EXIT_FAILURE);
    }

    if (1 != EVP_DecryptUpdate(ctx, (unsigned char *)decMsg, &len, (const unsigned char *)encrypted_ascii_message, length)) {
        printf("EVP_DecryptUpdate() failed\n");
        exit(EXIT_FAILURE);
    }

    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, (unsigned char *)(decMsg + len), &len)) {
        printf("EVP_DecryptFinal_ex() failed\n");
        exit(EXIT_FAILURE);
    }

    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    *(decMsg + plaintext_len) = '\0';

    *out = decMsg;

    return plaintext_len;
}

int Base64Encode(const char* buffer, size_t length, char** b64text) { //Encodes a binary safe base 64 string
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

int Base64Decode(char* b64message, char** buffer, size_t* length) { //Decodes a base64 encoded string
	BIO *bio, *b64;

	int decodeLen = calcDecodeLength(b64message);
	*buffer = (char*)malloc(decodeLen + 1);
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
