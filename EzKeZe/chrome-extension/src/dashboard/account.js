$(document).ready(function() {
    'use strict';

    /*
    $.validator.addMethod(
        "username",
        function(value) {
            return /^[A-Za-z0-9_-]{3,20}$/.test(value);
        },
        "Username can only contain [A-Z,a-z,0-9,_,-] and must be between 3 and 20 characters long"
    );
    $.validator.addMethod(
        "password",
        function(value) {
            return /^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)[a-zA-Z\d]{5,30}$/.test(value);
        }, 'Password must contain at least one number, one upper-case, and one lower-case character.' +
        'Also must be between 5 and 30 characters long'
    );
    */

    //  LOGIN DIALOG

    // load dialog
    var loginDialog = $('#loginDialog');
    dialogPolyfill.registerDialog(loginDialog["0"]);
    $('#loginButton').click(function(event) {
        loginDialog["0"].showModal();
    });
    /*
    loginDialog["0"].validate({
        rules: {
            loginUsername: {
                required: true,
                username: true
            },
            loginPassword: {
                required: true,
                password: true
            }
        }
    });
    */
    var loginUsernameField = $('#loginUsername');
    var loginPasswordField = $('#loginPassword');

    // load buttons
    var loginCloseButton = $('#loginCloseButton');
    loginCloseButton.click(function(event) {
        loginDialog["0"].close();
    });

    var loginSubmitButton = $('#loginSubmitButton');
    loginSubmitButton.click(function(event) {
      var xhttp = new XMLHttpRequest();
      xhttp.open("POST", "http://stoh.io:8080/api/login", true);
      xhttp.setRequestHeader("Content-type", "application/json");
      var username = loginUsernameField["0"].value.toString();
      var password = loginPasswordField["0"].value.toString();
      password = Sha256.hash(password);
      var loginJSON = {
        'username': username,
        'password': password
      };
      var loginString = JSON.stringify(loginJSON);
      xhttp.send(loginString);
      loginDialog["0"].close();
    });

    //  REGISTER DIALOG

    // load dialog
    var registerDialog = $('#registerDialog');
    dialogPolyfill.registerDialog(registerDialog["0"]);
    $('#registerButton').click(function(event) {
        registerDialog["0"].showModal();
    });
    /*
    registerDialog.validate({
        rules: {
            registerUsername: {
                required: true,
                username: true
            },
            registerPassword: {
                required: true,
                password: true
            },
            registerPasswordConf: {
                required: true,
                password: true,
                equalTo: '#registerPassword'
            }
        }
    });
    */
    var registerUsernameField = $('#registerUsername');
    var registerPasswordField = $('#registerPassword');
    var registerPasswordConfField = $('#registerPasswordConf');

    // load buttons
    var registerCloseButton = $('#registerCloseButton');
    registerCloseButton.click(function(event) {
        registerDialog["0"].close();
    });

    var registerSubmitButton = $('#registerSubmitButton');
    registerSubmitButton.click(function(event) {

        chrome.runtime.sendMessage({msg: "generate_rsa_keys"}, function(response) {
            console.log(response);
        });

        var xhttp = new XMLHttpRequest();
        xhttp.open('Post', 'http://stoh.io:8080/api/register', true);
        xhttp.setRequestHeader('Content-type', 'application/json');
        var username = registerUsernameField["0"].value.toString();
        var password = registerPasswordField["0"].value.toString();
        password = Sha256.hash(password);
        var registerJSON = {
            'username': username,
            'password': password
        };
        var registerString = JSON.stringify(registerJSON);
        xhttp.send(registerString);
        registerDialog["0"].close();
    });
});
