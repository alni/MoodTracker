/*
Mood Tracker v1.2
https://github.com/alni/MoodTracker/
----------------------
The MIT License (MIT)

Copyright (c) 2016 Alexander Nilsen

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
src/js/lib/mood_tracker.js
*/

var timeline = require("timeline.js");

/**
 * Format number with leading zero if less than 10
 * @param {integer} num - the number to format
 * @retuns {string} returns the number with leading
 *     zero if less than 10
 */
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
var createPin = function(time, duration, bwDisplay) {
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
      "primaryColor": bwDisplay ? "#FFFFFF" : "#000000",
      "secondaryColor": bwDisplay ? "#FFFFFF" : "#000000",
      "backgroundColor": bwDisplay ? "#000000" : "#55AAAA"
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
  var MoodTrackerPin = function(time, duration, reminders, platform) {
    this.time = time;
    this.duration = duration;
    this.id = generatePinId(time);
    this.reminders = reminders || [10, 14, 18, 22];
    this.platform = platform;
    this.debug = true;
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
      pin = createPin(time, this.duration, this.platform == "aplite");
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
  timeline.insertUserPin(pin, this.onInserted);
  return this;
};

var MoodTracker = {
  /**
   * @lends MoodTracker.Pin
   */
  Pin : (function() {
    return MoodTrackerPin;
  })(),
};

module.exports = MoodTracker;