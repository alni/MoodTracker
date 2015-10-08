/*
Mood Tracker v1.2
https://github.com/alni/MoodTracker/
----------------------
The MIT License (MIT)

Copyright (c) 2015 Alexander Nilsen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
--------------------
src/pebble-js-app.js
*/

var moodMinSetting = 'mood_min';
var moodMaxSetting = 'mood_max';
var moodStepSetting = 'mood_step';
var reminderDaysSetting = 'reminder_days';

Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('https://alni.github.io/MoodTracker/pebble/config.html');
});

Pebble.addEventListener('webviewclosed', function(e) {
  // Decode and parse config data as JSON
  var config_data = JSON.parse(decodeURIComponent(e.response));
  console.log('Config window returned: ' + JSON.stringify(config_data));
  // Prepare AppMessage payload
  var dict = {
    "KEY_MOOD_MIN": config_data[moodMinSetting],
    "KEY_MOOD_MAX": config_data[moodMaxSetting],
    "KEY_MOOD_STEP": config_data[moodStepSetting],
    "KEY_REMINDER_DAYS": config_data[reminderDaysSetting]
  };
  
  // Send settings to Pebble watchapp
  Pebble.sendAppMessage(dict, function() {
    console.log('Sent config data to Pebble');
  }, function() {
    console.log('Failed to send config data!');
  });
});


/**
 * @namespace
 */
var MoodTracker = (function() {
  var formatNumber = function (num) {
    return num < 10 ? "0" + num : "" + num;
  };
  /**
   * Generate Pin ID with a format based on the date
   * @param {Date} time - the time object to use
   * @returns {string} returns the ID with the format:
   *     "moodtracker-YYYY-MM-DD-HH"
   */
  var generatePinId = function(time) {
    var hours = formatNumber(time.getHours()),
        dom = formatNumber(time.getDate()),
        month = formatNumber(time.getMonth()),
        year = time.getFullYear();
    
    return ["moodtracker", year, month, dom, hours].join("-");
  };
  /**
   * Creates a Timeline Pin from a given time and duration
   * @param {Date} time - the time for the pin
   * @param {integer} duration - the duration of the pin in seconds
   * @returns {object} a Timeline Pin object
   */
  var createPin = function(time, duration) {
    return {
      "id": generatePinId(time),
      "time": time.toISOString(),
      "duration": duration,
      "createNotification": {
        "layout": {
          "type": "genericNotification",
          "title": "New Item",
          "tinyIcon": "system://images/NOTIFICATION_FLAG",
          "body": "A new Mood Tracker pin has been added to your timeline."
        }
      },
      "layout": {
        "title": "Mood Tracker",
        "type": "genericPin",
        "tinyIcon": "system://images/NOTIFICATION_REMINDER",
        "body": "Remember to log your mood.",
        "backgroundColor": "#55AAAA"
      },
      "reminders": [],
      "actions": [
        {
          "title": "Log your mood",
          "type": "openWatchApp",
          "launchCode": 15
        },
        {
          "title": "Review moods",
          "type": "openWatchApp",
          "launchCode": 22
        }
      ]
    };
  };
  
  /**
   * Creates a Timeline Pin reminder object
   * @param {Date} currentTime - the time to use as a base for the reminder
   * @param {integer} hour - the hour to set the reminder for
   * @param {integer} [minute] - the minute to set the reminder for
   * @returns {Object} a Timeline Pin reminder object
   */
  var createReminder = function(currentTime, hour, minute) {
    var time = new Date(+currentTime);
    time.setHours(hour);
    time.setMinutes(minute || 0);
    return {
      "time": time.toISOString(),
      "layout": {
        "type": "genericReminder",
        "tinyIcon": "system://images/NOTIFICATION_REMINDER",
        "title": "Reminder to log your mood"
      }
    };
  };
  
  /**
   * Constructs a new MoodTracker Pin
   * @class Pin
   * @memberof MoodTracker
   * @param {Date} time - the time to use for the pin
   * @param {integer} duration - the duration of the pin in seconds
   * @param {integer[]} reminders - the list of reminder hours for the pin
   */
  var MoodTrackerPin = function(time, duration, reminders) {
    this.time = time;
    this.duration = duration;
    this.id = generatePinId(time);
    this.reminders = reminders || [10, 14, 18, 22];
    this.debug = false;
  };
  
  /**
   * Creates a Timeline Pin
   * @returns {Object} a Timeline Pin object
   */
  MoodTrackerPin.prototype.getPin = function() {
    var time = this.time,
        reminders = this.reminders,
        l = reminders.length,
        i = 0,
        pin = createPin(time, this.duration);
    delete pin.createNotification;
    for (i = 0; i < l; i++) {
      pin.reminders.push(createReminder(time, reminders[i]));
    }
    return pin;
  };
  /**
   * Inserts the MoodTracker pin to the timeline
   * @fires MoodTracker.Pin#onInserted
   * @returns {MoodTracker.Pin} itself (allows chaining)
   */
  MoodTrackerPin.prototype.insertPin = function() {
    if (!this.onInserted) {
      this.onInserted = function() {};
    }
    var pin = this.getPin();
    if (this.debug) {
      console.log('Inserting pin in the future: ' + JSON.stringify(pin));
    }
    insertUserPin(pin, this.onInserted);
    return this;
  };

  return {
    /**
     * @lends MoodTracker.Pin
     */
    Pin : (function() {
      return MoodTrackerPin;
    })(),
    /**
     * Creates a Timeline Pin
     * @memberof MoodTracker
     * @param {Date} time - the time to use for the pin
     * @param {integer} duration - the duration of the pin in seconds
     * @returns {object} a Timeline Pin object
     */
    createPin : function(time, duration) {
      var pin = createPin(time, duration),
          reminderTime = new Date(),
          reminderHour = reminderTime.getHours();
      //var reminderHour = time.getHours();
      //pin.reminders.push(createReminder(reminderHour));
      pin.reminders.push(createReminder(time, 10));
      pin.reminders.push(createReminder(time, 14));
      pin.reminders.push(createReminder(time, 18));
      pin.reminders.push(createReminder(time, 22));
      if(Pebble.getActiveWatchInfo) {
        // Available for use!
        var watch = Pebble.getActiveWatchInfo();
        console.log(watch.model);
        if (watch.model.indexOf("qemu") >= 0) {
          /*for (var i = reminderHour; i < reminderHour+1; i++) {
            for (var j = 0; j < 59; j+=5) {
              pin.reminders.push(createReminder(time, i, j));
            }
          }*/
        }
      } else {
        // Not available, handle gracefully
      
      }
      
      return pin;
    }
  };
})();


var NUM_DAYS = 7; // Number of days to create pins for

Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS ready!');
  var time = new Date(),
      duration = 3600, // 1 hour
      times = [],
      hours = [10, 14, 18, 22],
      _time = null,
      i = 0,
      j = 0,
      length = 0,
      completed = 0,
      results = null,
      pin = null;
  time.setHours(10);
  time.setMinutes(0);
  for (i = 0; i < NUM_DAYS; i++) {
    for (j = 0; j < hours.length; j++) {
      _time = new Date(+time);
      _time.setDate(_time.getDate() + i);
      _time.setHours(hours[j]);
      console.log(_time.toString());
      times.push(_time); 
    }
  }
  length = times.length;
  results = new Array(length);
  pin = new MoodTracker.Pin(times[0], duration, [times[0].getHours()]);
  pin.onInserted = function(responseText) {
    results[completed] = responseText;
    if (++completed === length) {
      console.log('Result: ' + results.join(' ; '));
    } else {
      pin.time = times[completed];
      pin.reminders = [times[completed].getHours()];
      pin = pin.insertPin();
    }
  };
  //onsole.log('Inserting pin in the future: ' + JSON.stringify(pin));
  
  // Insert for today
  pin = pin.insertPin();
});

/******************************* timeline lib *********************************/

// The timeline public URL root
var API_URL_ROOT = 'https://timeline-api.getpebble.com/';

/**
 * Send a request to the Pebble public web timeline API.
 * @param pin The JSON pin to insert. Must contain 'id' field.
 * @param type The type of request, either PUT or DELETE.
 * @param callback The callback to receive the responseText after the request has completed.
 */
function timelineRequest(pin, type, callback) {
  // User or shared?
  var url = API_URL_ROOT + 'v1/user/pins/' + pin.id;

  // Create XHR
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    console.log('timeline: response received: ' + this.responseText);
    callback(this.responseText);
  };
  xhr.open(type, url);

  // Get token
  Pebble.getTimelineToken(function(token) {
    // Add headers
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.setRequestHeader('X-User-Token', '' + token);

    // Send
    xhr.send(JSON.stringify(pin));
    console.log('timeline: request sent.');
  }, function(error) { console.log('timeline: error getting timeline token: ' + error); });
}

/**
 * Insert a pin into the timeline for this user.
 * @param pin The JSON pin to insert.
 * @param callback The callback to receive the responseText after the request has completed.
 */
function insertUserPin(pin, callback) {
  timelineRequest(pin, 'PUT', callback);
}

/**
 * Delete a pin from the timeline for this user.
 * @param pin The JSON pin to delete.
 * @param callback The callback to receive the responseText after the request has completed.
 */
function deleteUserPin(pin, callback) {
  timelineRequest(pin, 'DELETE', callback);
}

/***************************** end timeline lib *******************************/


/* EXAMPLE JSON PIN 

{
  "id": "moodtracker-453923",
  "time": "2015-07-22T00:00:00Z",
  "duration": 1440,
  "createNotification": {
    "layout": {
      "type": "genericNotification",
      "title": "New Item",
      "tinyIcon": "system://images/NOTIFICATION_FLAG",
      "body": "A new appointment has been added to your calendar at 4pm."
    }
  },
  "layout": {
    "title": "Mood Tracker",
    "type": "genericPin",
    "tinyIcon": "system://images/TIMELINE_CALENDAR",
    "body": "Remember to log your mood."
  },
  "reminders": [
    {
      "time": "2015-07-22T08:00:00Z",
      "layout": {
        "type": "genericReminder",
        "tinyIcon": "system://images/TIMELINE_CALENDAR",
        "title": "Reminder to log your mood"
      }
    },
    {
      "time": "2015-07-22T12:00:00Z",
      "layout": {
        "type": "genericReminder",
        "tinyIcon": "system://images/TIMELINE_CALENDAR",
        "title": "Reminder to log your mood"
      }
    }, 
    {
      "time": "2015-07-22T16:00:00Z",
      "layout": {
        "type": "genericReminder",
        "tinyIcon": "system://images/TIMELINE_CALENDAR",
        "title": "Reminder to log your mood"
      }
    },
    {
      "time": "2015-07-22T20:00:00Z",
      "layout": {
        "type": "genericReminder",
        "tinyIcon": "system://images/TIMELINE_CALENDAR",
        "title": "Reminder to log your mood"
      }
    }
  ],
  "actions": [
    {
      "title": "Log your mood",
      "type": "openWatchApp",
      "launchCode": 15
    },
    {
      "title": "Review moods",
      "type": "openWatchApp",
      "launchCode": 22
    }
  ]
}

*/