var current_chat;
var current_chat_key;

current_chat_key = [115, 98, 103, 111, 108, 121, 112, 105, 101, 115, 114, 100, 97, 105, 105, 97];
/*
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
*/

chrome.commands.onCommand.addListener(function(command){

  if(command == 'encrypt_userText'){

    chrome.tabs.query({active: true, currentWindow: true}, function(tabs) {
      chrome.tabs.sendMessage(tabs[0].id, {action: "encrypt_request"}, function(response){
	      if(!response) console.log("response was null");
        console.log("We got a response");
        console.log(response.text);

        var ciphertext = encrypt_sym_message(response.text, current_chat_key);
        console.log(ciphertext);
        var encryptedmessage =  {'identifier': 'MmmmMMMMmmmm_Dat_Some_GOOOOOOOOOOD_encRYPTION_mmmmMMMMMmmmmMM', 'content': ciphertext};

        var messageString = JSON.stringify(encryptedmessage);
        var data = {action: 'encrypted_text', text: messageString};

        chrome.tabs.query({active: true, currentWindow: true}, function(tabs) {
            chrome.tabs.sendMessage(tabs[0].id, data, function(response) {
                console.log('The text should be encrypted');
            });
        });
    });

  });
  }
});

chrome.extension.onMessage.addListener(
  function(request, sender, sendResponse){
    console.log("Extenion message?");

    if(request.msg == "decrypt_this"){
      var array = request.content;
      console.log(array);
      plaintext = decrypt_sym_message(Uint8Array.from(array), current_chat_key);
      console.log("Encrypted Message: " + request.content);
      console.log("Decrypted Message: " + plaintext);
      sendResponse(plaintext);
    }
  }
)


//Code to decrypt the shit
//decrypt_sym_message(message, current_chat_key);
