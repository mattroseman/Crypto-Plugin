// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Called when the user clicks on the browser action.
console.log("Shit Happened");
chrome.browserAction.onClicked.addListener(function(tab) {
  // No tabs or host permissions needed!
  console.log('Hello World!');
  chrome.tabs.sendMessage(tab.id, {text: 'report_back'}, doStuffWithDom);

});

function doStuffWithDom(domContent) {
    console.log('I received the following DOM content:\n' + domContent);
}
