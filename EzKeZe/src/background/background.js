var current_chat;
var current_chat_key;

current_chat_key = generate_sym_key();

chrome.extension.onMessage.addListener(function(request, sender, sendResponse) {
    if (request.action == "encrypt") {
        var plaintext = request.text;
        var ciphertext = encrypt_sym_message(plaintext, current_chat_key);
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
        console.log("We got a response");
        console.log(response.text);
        //var key = generate_asym_keys("p@ssw0rd");
        //var publickey = cryptico.publicKeyString(key);
        //var ciphertext = encrypt_asym_message(response.text, publickey);
        var ciphertext = encrypt_sym_message(response.text, current_chat_key);
        console.log(ciphertext);
    });

  });
  }
});

