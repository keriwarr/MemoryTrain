var SetPebble_token = "VJEJ";
var settings_URL;

var xhrRequest = function (url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        callback(this.responseText);
    };
    xhr.open(type, url);
    xhr.send();
};

// listen for watchface being opened
Pebble.addEventListener("ready", function(e) {
    console.log("PebbleKit JS ready!");
    settings_URL = "http://x.setpebble.com/api/" + SetPebble_token + "/" + Pebble.getAccountToken();
});

// listen for receiving of AppMessage event
Pebble.addEventListener("appmessage", function(e) {
    console.log("Received AppMessage signal");
    xhrRequest(url, "GET", function(response_text) {
        console.log("Setting questions to " + response_text);
        var result_json = JSON.parse(response_text);
        Pebble.sendAppMessage(result_json, function(e) { console.log("Questions sent to Pebble successfully!"); }, function(e) { console.log("Error sending questions to Pebble!"); });
    }
});

// listen for the config window opening
Pebble.addEventListener('showConfiguration', function(e) { Pebble.openURL('http://x.SetPebble.com/' + setPebbleToken + '/' + Pebble.getAccountToken()); });

// listen for the config window being closed
Pebble.addEventListener('webviewclosed', function(e) {
    if (typeof(e.response) != 'string' || e.response.length == 0) return;
    try {
        console.log("Received question config settings " + e.response);
        Pebble.sendAppMessage(JSON.parse(e.response));
        localStorage.setItem(SetPebble_token, e.response);
    } catch(e) {}
});
