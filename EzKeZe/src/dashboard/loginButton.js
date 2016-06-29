'use strict';
var dialog = document.querySelector('#logInDialog');
var closeButton = dialog.querySelector('button');
var showButton = document.querySelector('#show-modal-example');
var email;

if (! dialog.showModal) {
    dialogPolyfill.registerDialog(dialog);
}

var logInEmailField = document.getElementById('logInEmail');
var logInPasswordField = document.getElementById('logInPassword');

var logInClickHandler = function(event) {
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "http://stoh.io:8080/api/login", true);
  xhttp.setRequestHeader("Content-type", "application/json");
  email = logInEmailField.value.toString();
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
    var regemail = registerEmail.value.toString();
    var regpassword = registerPassword.value.toString();
    regpassword = Sha256.hash(regpassword);
    console.log("password");

    var registerJSON = {
      'email': regemail,
      'password': regpassword,
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

var serverSyncButton = document.getElementById("serverSyncButton");
console.log(serverSyncButton)

var serverSyncClickHandeler = function(event) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function(e) {
      if (this.readyState == 4 && this.status == 200) {
        console.log(this.responseText);
      }
    };
    xhttp.open("POST", "http://stoh.io:8080/api/server_sync", true);
    xhttp.setRequestHeader("Content-type", "application/json");

    var serverSyncJSON = {
      'email': 'ig11'
    }

    var serverSyncString = JSON.stringify(serverSyncJSON);

    xhttp.send(serverSyncString);

};

serverSyncButton.addEventListener('click', serverSyncClickHandeler);
