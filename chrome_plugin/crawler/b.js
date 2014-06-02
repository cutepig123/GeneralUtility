chrome.browserAction.onClicked.addListener(
function (tab ) {
 var tablink = tab.url;
 chrome.tabs.executeScript({ file: 'c.js' }); 
 }
);