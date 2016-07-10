#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/var_dictionary.h"

#include <string.h>
#include <stdio.h>
#include <math.h>
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
int calcEncodedLength(float); //Calculates the lenght of an encoded string
int Base64Decode(char*, char**, size_t*); //Decodes a base64 encoded string

RSA *generate_rsa_keys();
unsigned int rsa_publickey_to_pem(RSA*, char**);
RSA *rsa_pem_to_publickey(char*);
unsigned int rsa_privatekey_to_pem(RSA*, char**, char*);
RSA *rsa_pem_to_privatekey(char*, RSA*);
char *generate_aes_key();
char *generate_aes_iv();
unsigned int rsa_encrypt_message(char*, unsigned int, RSA*, char**);
unsigned int rsa_decrypt_message(char*, RSA*, char**);
unsigned int aes_encrypt_message(char*, unsigned int, char*, char*, char**);
unsigned int aes_decrypt_message(char*, unsigned int, char*, char*, char**);

char *string2char_array(std::string);

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
                //pp::VarArray keys = dictionary.GetKeys();

                pp::VarDictionary var_reply;
                pp::Var test_reply;
                var_reply.Set(pp::Var("original_request"), var_message);

                char *password = string2char_array("P@ssw0rd");
                RSA *rsa_key = generate_rsa_keys();

                char *rsa_pubkey_pem;
                char *rsa_privkey_pem;
                rsa_publickey_to_pem(rsa_key, &rsa_pubkey_pem);
                rsa_privatekey_to_pem(rsa_key, &rsa_privkey_pem, password);

                fprintf(stdout, "%s\n", rsa_pubkey_pem);
                fprintf(stdout, "%s\n", rsa_privkey_pem);

                std::string public_key(rsa_pubkey_pem);
                std::string private_key(rsa_privkey_pem);

                var_reply.Set(pp::Var("public_key"), pp::Var(public_key));
                var_reply.Set(pp::Var("private_key"), pp::Var(private_key));

                char *plaintext = (char *)string2char_array("Hello World!");
                RSA *key = rsa_pem_to_publickey(rsa_pubkey_pem);
                char *rsa_encrypted_message = (char *)malloc(kBits);
                int encrypted_length = rsa_encrypt_message(plaintext, strlen(plaintext), key, &rsa_encrypted_message);

                std::string encrypted_message((char *)rsa_encrypted_message);
                std::string encrypted_message_length = std::to_string(encrypted_length);

                var_reply.Set(pp::Var("encrypted_message"), pp::Var(encrypted_message));
                var_reply.Set(pp::Var("encrypted_message_length"), pp::Var(encrypted_message_length));

                //  TODO possible point of error
                char *rsa_decrypted_message = (char *)malloc(kBits);
                fprintf(stdout, "test: rsa_decrypted_message allocated\n");
                key = rsa_pem_to_privatekey(rsa_privkey_pem, key);
                fprintf(stdout, "test: rsa_pem_to_privatekey worked\n");

                char *temp_pubkey_pem;
                char *temp_privkey_pem;
                rsa_publickey_to_pem(key, &temp_pubkey_pem);
                rsa_privatekey_to_pem(key, &temp_privkey_pem, password);

                fprintf(stdout, "%s\n", temp_pubkey_pem);
                fprintf(stdout, "%s\n", temp_privkey_pem);

                rsa_decrypt_message(rsa_encrypted_message, key, &rsa_decrypted_message);
                fprintf(stdout, "test: rsa message has been decrypted\n");

                std::string decrypted_message(rsa_decrypted_message);

                var_reply.Set(pp::Var("decrypted_message"), pp::Var(decrypted_message));

                /*
                //  if asked to generate an rsa key
                if (dictionary.Get(pp::Var("request_type")).AsString().compare("generate_rsa_keys") == 0) {
                    char *password = string2char_array(dictionary.Get(pp::Var("password")).AsString());
                    RSA *rsa_key = generate_rsa_keys();

                    char *rsa_pubkey_pem;
                    char *rsa_privkey_pem;
                    rsa_publickey_to_pem(rsa_key, &rsa_pubkey_pem);
                    rsa_privatekey_to_pem(rsa_key, &rsa_privkey_pem, password);

                    std::string public_key(rsa_pubkey_pem);
                    std::string private_key(rsa_privkey_pem);

                    var_reply.Set(pp::Var("public_key"), pp::Var(public_key));
                    var_reply.Set(pp::Var("private_key"), pp::Var(private_key));
                }

                //  if asked to encrypt a string with a given rsa key
                if (dictionary.Get(pp::Var("request_type")).AsString().compare("encrypt_rsa") == 0) {
                    char *rsa_pub_pem = string2char_array(dictionary.Get(pp::Var("public_key")).AsString());
                    char *plaintext = string2char_array(dictionary.Get(pp::Var("message")).AsString());
                    char *rsa_encrypted_message = (char *)malloc(kBits);
                    RSA *key = rsa_pem_to_publickey(rsa_pub_pem);
                    unsigned int encrypted_length = rsa_encrypt_message(plaintext, strlen(plaintext), key, &rsa_encrypted_message);

                    std::string encrypted_message(rsa_encrypted_message);

                    var_reply.Set(pp::Var("encrypted_message"), pp::Var(encrypted_message));
                }

                //  if asked to decrypt a string with a given rsa key
                if (dictionary.Get(pp::Var("request_type")).AsString().compare("decrypt_rsa") == 0) {
                    char *rsa_priv_pem = string2char_array(dictionary.Get(pp::Var("private_key")).AsString());
                    char *rsa_pub_pem = string2char_array(dictionary.Get(pp::Var("public_key")).AsString());
                    char *encrypted_message = string2char_array(dictionary.Get(pp::Var("message")).AsString());
                    //  TODO possible point of error
                    char *rsa_decrypted_message = (char *)malloc(kBits);
                    RSA *key = rsa_pem_to_publickey(rsa_pub_pem);
                    key = rsa_pem_to_privatekey(rsa_priv_pem, key);
                    rsa_decrypt_message(encrypted_message, strlen(encrypted_message), key, &rsa_decrypted_message);

                    std::string decrypted_message(rsa_decrypted_message);

                    var_reply.Set(pp::Var("decrypted_message"), pp::Var(decrypted_message));
                }

                //  if asked to generate an aes key
                if (dictionary.Get(pp::Var("request_type")).AsString().compare("generate_aes_keys") == 0) {
                    char *aes_key = generate_aes_key();
                    char *aes_iv = generate_aes_iv();

                    std::string key(aes_key);
                    std::string iv(aes_iv);

                    var_reply.Set(pp::Var("aes_key"), pp::Var(key));
                    var_reply.Set(pp::Var("aes_iv"), pp::Var(iv));
                }

                //  if asked to encrypt a string with a given aes key
                if (dictionary.Get(pp::Var("request_type")).AsString().compare("encrypt_aes") == 0) {
                    char *aes_key = string2char_array(dictionary.Get(pp::Var("aes_key")).AsString());
                    char *aes_iv = string2char_array(dictionary.Get(pp::Var("aes_iv")).AsString());
                    char *plaintext = string2char_array(dictionary.Get(pp::Var("message")).AsString());
                    char *aes_encrypted_message = (char *)malloc(strlen(plaintext) + AES_BLOCK_SIZE);
                    unsigned int aes_encrypted_length = aes_encrypt_message(plaintext, strlen(plaintext), aes_key, aes_iv, &aes_encrypted_message);

                    std::string encrypted_message(aes_encrypted_message);

                    var_reply.Set(pp::Var("encrypted_message"), pp::Var(encrypted_message));
                }

                //  if asked to decrypt a string with a given aes key
                if (dictionary.Get(pp::Var("request_type")).AsString().compare("decrypt_aes") == 0) {
                    char *aes_key = string2char_array(dictionary.Get(pp::Var("aes_key")).AsString());
                    char *aes_iv = string2char_array(dictionary.Get(pp::Var("aes_iv")).AsString());
                    char *encrypted_message = string2char_array(dictionary.Get(pp::Var("message")).AsString());
                    char *aes_decrypted_message = (char *)malloc(strlen(encrypted_message));
                    unsigned int aes_decrypted_length = aes_decrypt_message(encrypted_message, strlen(encrypted_message), aes_key, aes_iv, &aes_decrypted_message);

                    std::string decrypted_message(aes_decrypted_message);

                    var_reply.Set(pp::Var("decrypted_message"), pp::Var(decrypted_message));
                }
                */

                PostMessage(var_reply);
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
    std::string pem = "";

    if (!BIO_eof(pubKey)) {
        BIO_gets(pubKey, line, sizeof *pubKey);
        pem += "-----BEGIN PUBLIC KEY-----\n";
    }
    while (!BIO_eof(pubKey)) {
        BIO_gets(pubKey, line, sizeof *pubKey);
        pem += line;
    }

    *out = string2char_array(pem);

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
    BIO *privKey = BIO_new(BIO_s_mem());

    PEM_write_bio_RSAPrivateKey(privKey, key, NULL, NULL, 0, NULL, NULL);

    char line[65];
    int len = 0;
    std::string pem = "";

    if (!BIO_eof(privKey)) {
        BIO_gets(privKey, line, sizeof *privKey);
        pem += "-----BEGIN PRIVATE KEY-----\n";
    }
    while (!BIO_eof(privKey)) {
        BIO_gets(privKey, line, sizeof *privKey);
        pem += line;
    }

    fprintf(stdout, "size of privatekey: %d\n", pem.length());

    *out = string2char_array(pem);

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

    fprintf(stdout, "%s\n", private_key);
    fprintf(stdout, "length from BIO_puts: %d\n", len);
    
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
unsigned int rsa_encrypt_message(char *message, unsigned int length, RSA *key, char **out) {
    char *encrypted_message = (char *)malloc(RSA_size(key));
    char *encoded_message = (char *)malloc(RSA_size(key));
    int size;

    if ((size = RSA_public_encrypt(length, (const unsigned char *)message, (unsigned char *)encrypted_message, key, padding)) < 0) {
        printf("RSA_public_encrypt() failed\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "encrypted ascii: %s\n", encrypted_message);
    fprintf(stdout, "returned length: %d\n", size);

    if (Base64Encode(encrypted_message, size, (char **)&encoded_message) < 0) {
        printf("Base64Encode failed\n");
        exit(EXIT_FAILURE);
    }
    size = calcEncodedLength((float)size);

    *(encoded_message + size) = '\0';

    fprintf(stdout, "encoded message: %s\n", encoded_message);

    *out = encoded_message;

    return size;
}

/* 
 * Decyrpts an encrypted text and sets it to out
 * @param encrypted_message: the RSA encrypted ciphertext
 * @param length: the length of the original plaintext
 * @param key: the RSA key to decrypt with
 * @param out: the string to print plaintext to
 */
unsigned int rsa_decrypt_message(char *encrypted_message, RSA *key, char **out) {
    char *encrypted_ascii_message = (char *)malloc(key_length);
    size_t *length = (size_t *)malloc(sizeof(size_t));
    int size;
    
    if (Base64Decode(encrypted_message, &encrypted_ascii_message, length) < 0) {
        printf("Base64Decode failed\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "encrypted message decoded\n");
    fprintf(stdout, "%s\n", encrypted_ascii_message);
    fprintf(stdout, "returned length: %d\n", *length);

    char *decrypted_message = (char *)malloc(*length);

    if ((size = RSA_private_decrypt(*length, (unsigned char *)encrypted_ascii_message, (unsigned char *)decrypted_message, key, padding)) < 0) {
        printf("RSA_private_decrypt() failed\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "encrypted message decrypted\n");

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

int calcEncodedLength(float in_len) {
    //  in_len * 4/3 gets the base64 length
    //  by dividing by 4, rounding up, an multiplying by 4 we account for the padding
    return (int)4 * ceil((in_len*4/3)/4);
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

char *string2char_array(std::string in) {
    char *out = new char[in.length() + 1];
    strcpy(out, in.c_str());
    return out;
}

