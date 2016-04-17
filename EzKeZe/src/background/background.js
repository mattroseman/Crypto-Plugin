var current_chat;
var current_chat_key;
var current_chat_iv;

chrome.extension.onMessage.addListener(function(request, sender, sendResponse) {
    if (request.action == "encrypt") {
        var plaintext = request.text;
        var ciphertext = encrypt_sym_message(plaintext, current_chat_key, current_chat_iv);
        chrome.tabs.query({active: true, currentWindow: true}, function(tabs) {
            chrome.tabs.sendMessage(tabs[0].id, {action: "encrypted_text", text: ciphertext}, function(response) {
                console.log(response);
            });
        });
    }
    else if (request.action == "sync") {

    }
});

chrome.commands.onCommand.addListener(function(command){
  if(command == 'encrypt_userText'){

    chrome.tabs.query({active: true, currentWindow: true}, function(tabs) {
      chrome.tabs.sendMessage(tabs[0].id, {action: "encrypt_request"}, function(response){
        console.log(response);
        var key = generate_asym_keys("p@ssw0rd");
        var publickey = cryptico.publicKeyString(key);
        var ciphertext = encrypt_asym_message(response, publickey).cipher;
        console.log(ciphertext);
    });

  });
  }
});

var key = generate_asym_keys("p@ssw0rd");

var publickey = cryptico.publicKeyString(key);

console.log("public key:\n" + publickey);

var plaintext = "Hello World";

console.log("plaintext: " + plaintext);

var ciphertext = encrypt_asym_message(plaintext, publickey).cipher;

console.log("ciphertext: " + ciphertext);
