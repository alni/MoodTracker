/*
Mood Tracker v1.2
https://github.com/alni/MoodTracker/
----------------------
The MIT License (MIT)

Copyright (c) 2015-2016 Alexander Nilsen

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
src/js/app.js
*/

var MoodTracker = require('./lib/mood_tracker');

var KEY_MOOD_SUN = 0;
var KEY_MOOD_MON = 1;
var KEY_MOOD_TUE = 2;
var KEY_MOOD_WED = 3;
var KEY_MOOD_THU = 4;
var KEY_MOOD_FRI = 5;
var KEY_MOOD_SAT = 6;

var moodMinSetting = 'mood_min';
var moodMaxSetting = 'mood_max';
var moodStepSetting = 'mood_step';
var moodBackupSetting = 'mood_backup';
var reminderDaysSetting = 'reminder_days';
var reminderHoursSetting = 'reminder_hours';

var moodLoggedData = 'mood_data';

Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  //Pebble.openURL("http://192.168.0.8/pebble/config.html");
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
    "KEY_MOOD_BACKUP": config_data[moodBackupSetting],
    "KEY_REMINDER_DAYS": config_data[reminderDaysSetting]
  };
  
  localStorage[moodBackupSetting] = config_data[moodBackupSetting];
  
  // Store Reminder settings in localStorage on the mobile device
  localStorage[reminderDaysSetting] = config_data[reminderDaysSetting] + "";
  localStorage[reminderHoursSetting] = config_data[reminderHoursSetting].join(",");
  
  // Send settings to Pebble watchapp
  Pebble.sendAppMessage(dict, function() {
    console.log('Sent config data to Pebble');
  }, function() {
    console.log('Failed to send config data!');
  });
});

// Get AppMessage events
Pebble.addEventListener('appmessage', function(e) {
  // Get the dictionary from the message
  var dict = e.payload;

  console.log('Got message: ' + JSON.stringify(dict));
  
  var moods = (localStorage[moodLoggedData] || '0,0,0,0,0,0,0').split(',').map(Number);
  for (var i = KEY_MOOD_SUN; i <= KEY_MOOD_SAT; i++) {
    if (dict.hasOwnProperty(i)) {
      moods[i] = dict[i]; 
    }
  }
  
  localStorage[moodLoggedData] = moods.join(',');
});

var NUM_DAYS = 7; // Number of days to create pins for
var HOURS = [10, 14, 18, 22]; // Hours to create pins for
var BACKUP = 0; // The Backup/Restore flag

Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS ready!');
  
  if (localStorage.getItem(moodBackupSetting) !== null) {
    BACKUP = +localStorage[moodBackupSetting];
  }
  
  if (BACKUP == 1) { // Only restore if the user wishes so
    // Load Mood Data from LocalStorage on the mobile device
    var moods = (localStorage[moodLoggedData] || "0,0,0,0,0,0,0").split(",").map(Number);
    //moods = [ 7, 8, 7, 7, 5, 6, 5 ];
    var dict = {};
    // Loop through each mood and add them to dict
    for (var mood_day = 0; mood_day < moods.length; mood_day++) {
     dict[mood_day] = moods[mood_day];
    }
    // Send the moods (in "dict") to the Pebble
    Pebble.sendAppMessage(dict, function() {
      console.log('Sent config data to Pebble');
    }, function() {
      console.log('Failed to send config data!');
    });
  }
  
  // Load Reminder settings from localStorage on the mobile device
  // Or use default values of nothing is stored.
  
  NUM_DAYS = localStorage[reminderDaysSetting] || NUM_DAYS;
  HOURS = localStorage[reminderHoursSetting] ? 
    localStorage[reminderHoursSetting].split(",").map(Number) 
    : HOURS;
  
  var now = +new Date(),
      time = new Date(),
      duration = 60, // 1 hour (60 minutes)
      times = [],
      hours = HOURS, //[10, 14, 18, 22],
      _time = null,
      i = 0,
      j = 0,
      length = 0,
      completed = 0,
      results = null,
      pin = null,
      platform = "aplite";
  if(Pebble.getActiveWatchInfo) {
    // Available for use!
    var watch = Pebble.getActiveWatchInfo();
    platform = watch.platform + "";
  } else {
    // Not available, handle gracefully
  }
  //time.setHours(10);
  time.setMinutes(0);
  for (i = 0; i < NUM_DAYS; i++) {
    for (j = 0; j < hours.length; j++) {
      _time = new Date(+time);
      _time.setDate(_time.getDate() + i);
      _time.setHours(hours[j]);
      _time.setMilliseconds(0);
      console.log(_time.toString());
      if (+_time > now) {
        // Only add times that are in the future
        times.push(_time);
      }
    }
  }
  length = times.length;
  results = new Array(length);
  pin = new MoodTracker.Pin(times[0], duration, [times[0].getHours()], platform);
  pin.onInserted = function(responseText) {
    // Insert another Timeline Pin after the previous as been inserted
    
    // Add the response from the inserted pin to results
    results[completed] = responseText;
    if (++completed === length) {
      // After all Pins has been inserted log the combined results
      console.log('Result: ' + results.join(' ; '));
    } else {
      // If there are still Pins that need to be inserted, use the same
      // MoodTracker Pin object, but set the Pin time and remnders of the 
      // next Pin in line
      pin.time = times[completed];
      pin.reminders = [times[completed].getHours()];
      // Re-set the MoodTracker Pin object to the MoodTracker Pin object 
      // returned when inserting the Pin to the Timeline
      pin = pin.insertPin();
      // Because we are using the same MoodTracker Pin object, this same
      // "onInserted" event callback will be called when this Pin has been
      // inserted to the Timeline
    }
  };
  //onsole.log('Inserting pin in the future: ' + JSON.stringify(pin));
  
  // Insert for today
  pin = pin.insertPin();
});

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