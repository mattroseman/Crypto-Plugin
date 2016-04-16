// must import openpgp module before using these functions

/**
 * generates PGP public and private keys
 * @param: userId the unique user identifier
 * @param: pass the user chosen passphrase
 * @return: an array consisting of public key and private key
 */
function generate_asym_keys(userId, pass) {
    options = {
        userIds: [{name: userId, email: ''}],
        numBits: 2048,
        passphrase: pass
    };

    openpgp.generateKey(options).then(function(key) {
        privkey = key.privateKeyArmored;
        pubkey = key.publicKeyArmored;

        return [pubkey, privkey];
    });
}

/**
 * takes a message and encrypts it using PGP RSA
 * @param: plaintext the unencrypted message
 * @param: pubkey the key used to encrypt
 * @return: the uint8array thats the encrypted message
 */
function encrypt_asym_message(plaintext, pubkey) {
    options = {
        data: Uint8Array.from(plaintext),
        publicKeys: openpgp.key.readArmored(pubkey).keys,
        armor: false
    };

    openpgp.encrypt(options).then(function(message) {
        encrypted = message.packets.write();

        return encrypted;
    });
}

/**
 * decrypts an encrypted message using PGP RSA
 * @param: ciphertext the encrypted message
 * @param: privkey the private key to decrypt
 * @return: the uint8array thats the plaintext message
 */
function decrypt_asym_message(ciphertext, privkey) {
    options = {
        message: openpgp.message.read(ciphertext),
        privateKey: openpgp.key.readArmored(privkey).keys[0],
        format: 'binary'
    };

    openpgp.decrypt(options).then(function(plaintext) {
        return plaintext;
    });
}
