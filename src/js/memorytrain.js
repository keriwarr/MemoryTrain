var SetPebble_token = "VJEJ";
var settings_URL;
var local_deck = [ // Hard code the things here instead of on the phone (assuming we get the messages flowing).
        ["What year was the University of Waterloo established?", "1957", 5],
        ["What is everyone's favourite hackathon?", "Hack the North", 5],
        ["What is the airspeed of an unladen swallow?", "11 m/s (European)", 5],
        ["How fast did Han Solo make the Kessel Run?", "12 Parsecs", 5],
        ["Who created XKCD?", "Randall Munroe", 5],
        ["Who played the Tenth Doctor?", "David Tennant", 5],
]
var next_question = local_deck[2]

var xhrRequest = function (url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        callback(this.responseText);
    };
    xhr.open(type, url);
    xhr.send();
};

// Update the timing interval of a question stored in the local deck.
function update_local_deck(question, remembered) {
    var local_deck = localStorage.get('deck')
    if (local_deck[question]) { // Make sure question is in the local deck.
        // Spaced repition algorithm: if user remembers answer, divide the time interval by two; else, multiply the time interval by two.
        if (remembered) {
            local_deck[question]['interval'] *= 2;
        } else {
            local_deck[question]['interval'] /= 2;
        }
    }
    localStorage.setItem('deck', local_deck); // Update local storage with newly-updated JSON array.
}

function update_next_question() {
    var len = local_deck.length
    var min = local_deck[0][2];
    var next = local_deck[0];

    // Search through local deck to find the smallest time interval. That will be the next question displayed.
    for (i=0; i<len; i++) {
        if (local_deck[i][2] < min) {
            min = local_deck[i][2];
            next = local_deck[i];
        }
    }

    return next;
}

// listen for watchface being opened
Pebble.addEventListener("ready", function(e) {
    console.log("PebbleKit JS ready!");
    settings_URL = "http://x.setpebble.com/api/" + SetPebble_token + "/" + Pebble.getAccountToken();
    localStorage.setItem('deck', local_deck); // Initialize local storage with original deck.
});

// listen for receiving of AppMessage event
Pebble.addEventListener("appmessage", function(e) {
    console.log("Received AppMessage signal");

    // Accept payload of message from Pebble
    var message = e.payload; // Not sure what this 'payload' looks like, just assuming it's a string.
    // Message should be in the form 'remembered' or 'forgotten'
    if (message === 'remembered') {
        // Move the next appearance of this question farther away.
        update_local_deck(message.question, true);
    } elif (message === 'forgotten') {
        // Make the question appear again sooner.
        update_local_deck(message.question, false);
    }

    // Find smallest time interval in the deck and make that question the next one.
    update_next_question();

    console.log("Setting next question to " + next_question[0]);
    
    Pebble.sendAppMessage(
        next_question,  // Send the next question (array of question, answer, interval) to the Pebble.
        function(e) {
            console.log("Question sent to Pebble successfully!");
        },
        function(e) {
            console.log("Error sending question to Pebble!");
        }
    );
});

function get_deck_from_cloud() {
    // var response_json = JSON.parse(response_text);
    xhrRequest(url, "GET", function(response_text) {
        // TO DO: Update local deck.
    }
}

// TO DO: function post_deck_to_cloud() {}

// listen for the config window opening
Pebble.addEventListener('showConfiguration', function(e) {
    Pebble.openURL('http://x.SetPebble.com/' + SetPebble_token + '/' + Pebble.getAccountToken());
});

// listen for the config window being closed
Pebble.addEventListener('webviewclosed', function(e) {
    if (typeof(e.response) != 'string' || e.response.length == 0) return;
    try {
        console.log("Received question config settings " + e.response);
        Pebble.sendAppMessage(JSON.parse(e.response));
        localStorage.setItem(SetPebble_token, e.response);
    } catch(e) {}
});