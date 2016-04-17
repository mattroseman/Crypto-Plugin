$(document).ready(function() {

    var key = generate_asym_keys("p@ssw0rd");

    var publickey = cryptico.publicKeyString(key);

    console.log("public key:\n" + publickey);

    var plaintext = "Hello World";

    console.log("plaintext: " + plaintext);

    var ciphertext = encrypt_asym_message(plaintext, publickey).cipher;

    console.log("encrypted message: " + ciphertext);

    $("#encrypt").click(function() {
      console.log("Encrypt!");
        // sends a message to te content scripts saying to encrypt the active text element
        var request = JSON.parse("{\"action\": \"encrypt\"}");

        chrome.tabs.query({active: true, currentWindow: true}, function(tabs) {
            chrome.tabs.sendMessage(tabs[0].id, request, function(response) {
                console.log(response);
            });
        });
    });

    $("#sync").click(function() {
        // sends a message to the background script saying to sync this users chat keys
        var request = JSON.parse("{\"action\": \"sync\"}");

        chrome.extension.sendMessage(request, function(response) {
            console.log(response);
        });
    });
});
