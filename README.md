# Crypto-Plugin


Core Features
- email(unique handle)/name/password registration
- Automatic encryption/decryption
- Backend hiding all keys
- local storage for private key, and all public keys

Stretch Goals
- support signed messages (default not signed)
- hashed password storeage
- a way to talk to someone who isnt registered
- closed channel chat app between two users
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


Server-Side
- Web Server to hold unique identifiers for private keys, and the corresponding public keys for each person
- | id | hashed-priv-FK | pub-key-FK | user | 
- | id | pub key |
- | id | hashed-priv | 
<br>
RESTful API supports  
- Given username, get pub key
- Store priv/pub key pairs
- user registration
- user login - downloads all keys related to user to their localDB
<br>
LocalDB to store priv-key and any pub keys
- | id | priv key | 
- | id | pub key | user
- hashed password storeage
- hashed password storeage





#References
https://developer.mozilla.org/en-US/docs/Web/API/IndexedDB_API/Using_IndexedDB#Creating_or_updating_the_version_of_the_database
<br>
http://blog.teamtreehouse.com/create-your-own-to-do-app-with-html5-and-indexeddb
<br>
https://trackingjs.com/
<br>
https://www.wanderu.com/en/depart/Columbus%2C%20OH%2C%20USA/Toledo%2C%20OH%2C%20USA/2016-04-15
