/* openssl headers */
#include "include/openssl/rsa.h"
#include "include/openssl/bn.h"

RSA* generate_rsa_keys();
void print_rsa_keys(RSA*, FILE*);

const int kBits = 1024; //  the length of the private key
const unsigned long kExp_long = 65537;

BIGNUM *kExp = NULL;

RSA *rsa; //  the RSA keys

char* rsa_key_filename = "rsa_key.txt"; //  the file to print the RSA keys to


int main() {
    printf("program start\n");

    kExp = BN_new();
    BN_set_word(kExp, kExp_long);

    printf("Big Number created and assigned value\n");

    rsa = generate_rsa_keys();

    printf("RSA key created and assigned\n");

    //  create the file to print the rsa key to
    FILE *rsa_key_file = fopen(rsa_key_filename, "w+"); 
    printf("rsa key file created\n");
    print_rsa_keys(rsa, rsa_key_file);
    printf("rsa key printed to file\n");

    exit(EXIT_SUCCESS);
}

/*
 * Generates some RSA keys
 * @return: RSA* object
 */
RSA* generate_rsa_keys() {
    //  generate key pair and store in rsa
    printf("generate_rsa_keys() method entered\n");

    RSA *rsa_temp = RSA_new();

    printf("rsa_temp created\n");

    if (RSA_generate_key_ex(rsa_temp, kBits, kExp, 0) < 0)
    {
        printf("RSA_generate_key_ex failed\n");
        exit(EXIT_FAILURE);
    }

    printf("rsa_temp has been generated\n");

    //  XXX this is unused now but may be used later
    /*
    unsigned char* e_b = (unsigned char*)calloc(RSA_size(rsa_temp), sizeof(unsigned char));
    //  converts public key to binary
    int e_size = BN_bn2bin(rsa_temp->e, e_b);
    */

    return rsa_temp;
}

/*
 * Prints some RSA keys to a file
 * @param rsa: the keys to print
 * @param file: the file to print to
 */
void print_rsa_keys(RSA* rsa, FILE* file) {
    RSA_print_fp(file, rsa, 0);
}
