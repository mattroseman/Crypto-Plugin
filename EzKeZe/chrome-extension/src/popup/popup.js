$(document).ready(function() {

    //  get the login status from background
    var request = JSON.parse("{\"action\": \"get_login_status\"}");

    chrome.runtime.sendMessage(request, function(response) {
        console.log("user logged in status: " + response);
        //$('#signup').hide();
        if (response === true) {
            //  if user is logged in hide login and show menu
            $('#login').hide();
        } else {
            //  if user isn't logged in, hide menu and show login
            $('#menu').hide();
        }
    });

    $('#sign-up').click(function() {
        console.log("signing up for EzKeZe");
        chrome.tabs.create({url: chrome.extension.getURL('src/dashboard/index.html')});
        /*
        $('#login').hide();
        $('#menu').hide();
        $('#signup').show();
        */
    });


    /*
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
    */
});

function login(form) {
    username = form.username.value;
    password = form.password.value;
    //  send this to background to check password, and decrypt private key
    //  TODO possibly hash password here, or in background (probably background so OpenSSL can be used
    var request = JSON.parse("{\"action\": \"verify_password\"}");
    chrome.runtime.sendMessage(request, function(response) {
        console.log("user password status: " + response);

        //  if the password and username are correct
        if (response === true) {
        //  if the password and username don't match
        } else {
        }
    });
}

/*
function signup() {
    username = form.username.value;
    password = form.password.value;
    password_confirm = form.password_confirm.value;
}
*/
