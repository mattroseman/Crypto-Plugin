// Listen for messages
//nogf

chrome.runtime.onMessage.addListener(
  function (msg, sender, sendResponse) {
    //Msg is the message, sender is the sender of the message, and sendResponse is the callback function


    //The extension wants to know what to encrypt
    if(msg.action == "encrypt_request"){
      //Get the selected text
      var active = document.activeElement;
      var txt = textNodesUnder(active);
      var selectedText = window.getSelection().toString();
      //If there is text selected
      if(selectedText){
        console.log(selectedText);
        //Send the text back to the extension
        sendResponse({text: selectedText, action: 'pleaseEncrypt'});
      }
      /*else if(txt[0].baseURI.search('hangouts.google.com') == -1 && txt[0].nodeValue && txt[0].nodeValue.search("init()") == -1){
        console.log(txt);
        console.log(txt[0].nodeValue);
        sendResponse(txt[0].nodeValue);
      }
      */
    }
    //The extension sent a completed encryption
    if(msg.action == 'encrypted_text'){
      //Replaces the selected text with the txt in the message
      replaceSelectedText(msg.text);
    }
    //The extension wants to decrypt all the text on the DOM
    if(msg.action == 'decrypt_request'){
      //The identifier should be changed sometime
      findEncryptedText(document.body, "MmmmMMMMmmmm_Dat_Some_GOOOOOOOOOOD_encRYPTION_mmmmMMMMMmmmmMM");
      sendResponse("Finished Decrypting");
    }



});

//Old code for automatically decrypting, might be useful someday
document.addEventListener('DOMNodeInserted', nodeInsertedCallback);

function nodeInsertedCallback(event){
  var node2search = event.target;

  findText(event.target, "test");
  //findEncryptedText(node2search, "MmmmMMMMmmmm_Dat_Some_GOOOOOOOOOOD_encRYPTION_mmmmMMMMMmmmmMM");

}



 function textNodesUnder(el){
  var n, a=[], walk=document.createTreeWalker(el,NodeFilter.SHOW_TEXT,null,false);
  while(n=walk.nextNode()) a.push(n);
  return a;
}


//Algorithims for searching the page
var findText = function(node, text){
  var textNodes = textNodesUnder(node);
  for(i in textNodes){
    if(textNodes[i].textContent.search(text) != -1){

    }
  }
}

function findEncryptedText(searchNode, regexString)  {

    if (searchNode.nodeName == "MARK" ||
        searchNode.nodeName == "SCRIPT" ||
        searchNode.nodeName == "NOSCRIPT" ||
        searchNode.nodeName == "STYLE" ||
        searchNode.nodeType == Node.COMMENT_NODE) {

            return;
    }

    // if this is a nonempty text node
    if (searchNode.nodeType === Node.TEXT_NODE && (/[^\t\n\r ]/.test(searchNode.nodeValue))) {
        var nodeValue, before, match, after; // string value of nodes
        var regex; // the Regular Expression
        var result; // result of regex.exec("string");
        var parentNode;
        do {
            nodeValue = searchNode.nodeValue;
            regex = new RegExp(regexString, "g");
            result = regex.exec(nodeValue);
            // if a match was found
            if (result != null) {

                parentNode = searchNode.parentNode;
                //Create an object from the encrypted text
                var encryptObject = JSON.parse(nodeValue);
                //Add a msg to the object that you want to decrypt it
                encryptObject.msg = "decrypt_this";
                console.log(encryptObject);
                console.log(parentNode);
                console.log(searchNode);
                //Send the encrypted text object to the extension to be decoded
                chrome.extension.sendMessage(encryptObject, function(response){
                    console.log(response);
                    parentNode.innerHTML = response;
                });

                console.log("Here we go!")
                console.log(parentNode);

                match = nodeValue; // the string of matched text

                console.log("The string: " + match);
                break;
                }
            }
         while (result != null);
    } else {
        // So for some text boxes the hidden tag is set, so I'm going to continue to check everything hidden or not unless I figure out a better solution
        // if this is a hidden node or not displayed in on screen
        /*if ($(searchNode).css('display') == 'none' || $(searchNode).css('display') == 'hidden') {
           return;
        }*/
    }

    searchNode = searchNode.firstChild;
    //Recursively loop through the document
    while (searchNode) {
        findEncryptedText(searchNode, regexString);
        searchNode = searchNode.nextSibling;
    }

    return;
}

//Replace the findEncryptedTexted text with replacement text
function replaceSelectedText(replacementText) {
    var sel, range;
    if (window.getSelection) {
        sel = window.getSelection();
        if (sel.rangeCount) {
            range = sel.getRangeAt(0);
            range.deleteContents();
            range.insertNode(document.createTextNode(replacementText));
        }
    } else if (document.selection && document.selection.createRange) {
        range = document.selection.createRange();
        range.text = replacementText;
        if(document.getElementsByClassName('data-text').size != 0){

        }
    }
}
