// Listen for messages
chrome.runtime.onMessage.addListener(
  function (msg, sender, sendResponse) {
    console.log("message received");
    if(msg.action = "encrypt_request"){
      var active = document.activeElement;
      console.log(active);
      var txt = textNodesUnder(document.activeElement);
      console.log("GET ON MY LEVEL BOYS");
      console.log(txt);
      sendResponse(txt[0].nodeValue);
    }

});

document.addEventListener('DOMNodeInserted', nodeInsertedCallback);

function nodeInsertedCallback(event){
  var node2search = event.target;

  //findText(event.target, "test");
  highlight(node2search, "test");

}

/**
 * Takes html object and regex, and highlights the text that matches the regex
 * @param html the html object that will be searched
 * @param regex the regular expression to match to the html text
 */


 function textNodesUnder(el){
  var n, a=[], walk=document.createTreeWalker(el,NodeFilter.SHOW_TEXT,null,false);
  while(n=walk.nextNode()) a.push(n);
  return a;
}

var findText = function(node, text){
  var textNodes = textNodesUnder(node);
  for(i in textNodes){
    if(textNodes[i].textContent.search(text) != -1){
      console.log(textNodes[i]);
    }
  }
}


function highlight(searchNode, regexString)  {

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
        var beforeNode, matchNode, afterNode; // the new nodes being added
        var span; // the node being added above the match for highlighting
        do {
            nodeValue = searchNode.nodeValue;
            regex = new RegExp(regexString, "g");
            result = regex.exec(nodeValue);
            // if a match was found
            if (result != null) {

                parentNode = searchNode.parentNode;
                searchNode.nodeValue = "Nothing to see here";
                console.log("Here we go!")
                console.log(parentNode);

                match = nodeValue; // the string of matched text

                console.log("The string: " + match);


                matchNode = document.createTextNode(match);
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
    while (searchNode) {
        highlight(searchNode, regexString);
        searchNode = searchNode.nextSibling;
    }

    return;
}
highlight(document.body, "test");

function scanforIframes(){
  var iframes = document.getElementsByTag("iframe");
}
