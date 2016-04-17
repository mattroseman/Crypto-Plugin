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
    })
  });
  }
})
