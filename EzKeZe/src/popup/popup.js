$(document).ready(function() {
    $("#encrypt").click(function() {
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
