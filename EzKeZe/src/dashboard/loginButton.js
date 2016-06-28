'use strict';
var dialog = document.querySelector('#logInDialog');
var closeButton = dialog.querySelector('button');
var showButton = document.querySelector('#show-modal-example');

if (! dialog.showModal) {
    dialogPolyfill.registerDialog(dialog);
}

var logInEmailField = document.getElementById('logInEmail');
var logInPasswordField = document.getElementById('logInPassword');

var logInClickHandler = function(event) {
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "http://stoh.io:8080/api/login", true);
  xhttp.setRequestHeader("Content-type", "application/json");
  var email = logInEmailField.value.toString();
  var password = logInPasswordField.value.toString();
  password = Sha256.hash(password);
  var loginJSON = {
    'email': email,
    'password': password
  }
  var loginString = JSON.stringify(loginJSON);
  xhttp.send(loginString);
  dialog.close();
};

var showClickHandler = function(event) {
    dialog.showModal();
};

showButton.addEventListener('click', showClickHandler);
closeButton.addEventListener('click', logInClickHandler);


var registerDialog = document.querySelector('#registerDialog');
var registerCloseButton = registerDialog.querySelector('button');
var registerShowButton = document.querySelector('#registerButton');
var registerEmail = document.getElementById('registerEmail');
var registerPassword = document.getElementById('registerPassword');

if (! dialog.showModal) {
    dialogPolyfill.registerDialog(dialog);
}

var registerClickHandeler = function(event) {
    var xhttp = new XMLHttpRequest();
    xhttp.open("POST", "http://stoh.io:8080/api/register", true);
    xhttp.setRequestHeader("Content-type", "application/json");
    var email = registerEmail.value.toString();
    var password = registerPassword.value.toString();
    password = Sha256.hash(password);
    console.log("password");

    var registerJSON = {
      'email': email,
      'password': password,
      'gender': 'M',
      'public_key': 'alphaTest'
    }

    var registerString = JSON.stringify(registerJSON);

    xhttp.send(registerString);
    registerDialog.close();
};

var showClickHandler = function(event) {
    registerDialog.showModal();
};

registerShowButton.addEventListener('click', showClickHandler);
registerCloseButton.addEventListener('click', registerClickHandeler);
