# Crypto-Plugin


Core Features
- email(unique handle)/name/password registration
- Automatic encryption/decryption
- Backend hiding all keys
- local storage for private key, and all public keys

Stretch Goals
- a way to talk to someone who isnt registered
- closed channel chat app between two users
- hashed password storeage
- increases of security on server
- send encrypted files
- group encrypted communication


Auto Encrypt/Decrypt
- 3 field regex
- 1st field: pub key that was used to encrypt the msg(search for correspondong priv key to decode)
- 2nd field: pub key that should be used to encrypt a response(search for it on localdb, if doesnt exist then ping server and get it)
- 3rd field: the actual message to be decoded


API's used
- encryption in js: https://github.com/openpgpjs/openpgpjs
- password storage hash: https://www.npmjs.com/package/password-hash
