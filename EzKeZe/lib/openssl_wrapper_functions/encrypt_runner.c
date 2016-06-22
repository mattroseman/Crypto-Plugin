#include "encrypt.c"

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
}
