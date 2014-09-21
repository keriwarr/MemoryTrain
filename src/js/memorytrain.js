var SetPebble_token = "VJEJ";
var settings_URL;
var local_deck = [ // hard code the things here instead of on the phone (assuming we get the messages flowing).
        ["What year was the University of Waterloo established?", "1957", 5],
        ["What is everyone's favourite hackathon?", "Hack the North", 5],
        ["What is the airspeed of an unladen swallow?", "11 m/s (European)", 5],
        ["How fast did Han Solo make the Kessel Run?", "12 Parsecs", 5],
        ["Who created XKCD?", "Randall Munroe", 5],
        ["Who played the Tenth Doctor?", "David Tennant", 5],
]
var next_question = local_deck[2];

// update the timing interval of a question stored in the local deck
function update_local_deck(question, remembered) {
    // spaced repetition algorithm: if user remembers answer, divide the time interval by two; else, multiply the time interval by two
    console.log("Question " + question + " has remembered state " + remembered);
    var local_deck = localStorage.getItem("deck");
    if (local_deck[question]) // make sure question is in the local deck
        local_deck[question]["interval"] *= remembered ? 2 : 0.5;
    localStorage.setItem("deck", local_deck); // update local storage with newly-updated JSON array
}

function get_next_question() {
    var len = local_deck.length
    var min = local_deck[0][2]; // smallest time interval
    var next = local_deck[0];

    // search through local deck to find the smallest time interval. That will be the next question displayed.
    for (i = 0; i < len; i ++) {
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
    localStorage.setItem("deck", local_deck); // initialize local storage with original deck.
});

// listen for receiving of AppMessage event
Pebble.addEventListener("appmessage", function(e) {
    console.log("Received AppMessage signal");

    // accept AppMessage from Pebble
    var message = e.payload;
    if (message[0] === "remembered") // move the next appearance of this question farther away
        update_local_deck(message[1], true);
    else if (message[0] === "forgotten") // make the question appear again sooner
        update_local_deck(message[1], false);

    // find smallest time interval in the deck and make that question the next one
    next_question = get_next_question();

    console.log("Setting next question to " + next_question);
    
    Pebble.sendAppMessage(
        next_question,  // Send the next question (array of question, answer, interval) to the Pebble.
        function(e) { console.log("Question sent to Pebble successfully!"); },
        function(e) { console.log("Error sending question to Pebble!"); }
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
Pebble.addEventListener("showConfiguration", function(e) {
    Pebble.openURL("http://x.setpebble.com/" + SetPebble_token + "/" + Pebble.getAccountToken());
});

// listen for the config window being closed
Pebble.addEventListener("webviewclosed", function(e) {
    if (typeof(e.response) != "string" || e.response.length == 0) return;
    try {
        console.log("Received question config settings " + e.response);
        Pebble.sendAppMessage(JSON.parse(e.response));
        localStorage.setItem(SetPebble_token, e.response);
    } catch(e) {}
});
