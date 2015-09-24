var MoodTracker = (function() {
  var createPin = function(time, duration) {
    return {
      "id": "moodtracker-" + (parseInt(+time/(1000 * 60 * 60))),
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
  
  var MoodTrackerPin = function(time, duration, reminders) {
    this.time = time;
    this.duration = duration;
    this.id = parseInt(+time / 3600000);
    this.reminders = reminders || [10, 14, 18, 22];
  };
  
  MoodTrackerPin.prototype.getPin = function() {
    var time = this.time,
        reminders = this.reminders,
        l = reminders.length,
        i = 0;
    var pin = createPin(time, this.duration);
    delete pin.createNotification;
    for (; i < l; i++) {
      pin.reminders.push(createReminder(time, reminders[i]));
    }
    return pin;
  };
  MoodTrackerPin.prototype.insertPin = function() {
    if (!this.onInserted) {
      this.onInserted = function() {};
    }
    var pin = this.getPin();
    console.log('Inserting pin in the future: ' + JSON.stringify(pin));
    insertUserPin(pin, this.onInserted);
    return this;
  };


  return {
    Pin : (function() {
      return MoodTrackerPin;
    })(),
    createPin : function(time, duration) {
      var pin = createPin(time, duration);
      var reminderTime = new Date();
      var reminderHour = reminderTime.getHours();
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

Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS ready!');

  // An hour ahead
  //var date = new Date();
  //date.setHours(date.getHours() + 1);

  // Create the pin
  /*var pin = {
    "id": "pin-" + Math.round((Math.random() * 100000)),
    "time": date.toISOString(),
    "layout": {
      "type": "genericPin",
      "title": "Example Pin",
      "tinyIcon": "system://images/SCHEDULED_EVENT"
    }
  };*/
  var time = new Date();
  var currentHours = time.getHours();
  var endTime = new Date();
  /*if (currentHours > 18) {
    time.setHours(22);
    endTime.setHours(23);
  } else if (currentHours > 14) {
    time.setHours(18);
    endTime.setHours(21);
  } else if (currentHours > 10) {
    time.setHours(14);
    endTime.setHours(17);
  } else {
    time.setHours(10);
    endTime.setHours(13);
  }*/
  time.setHours(10);
  time.setMinutes(0);
  endTime.setHours(22);
  endTime.setMinutes(59);
  
  
  var times = [],
      i = 0,
      _time = null,
      hours = [10, 14, 18, 22],
      j = 0;
  for (i = 0; i < 3; i++) {
    for (j = 0; j < hours.length; j++) {
      _time = new Date(+time);
      _time.setDate(_time.getDate() + i);
      _time.setHours(hours[j]);
      console.log(_time.toString());
      times.push(_time); 
    }
  }
  //var duration = +endTime - (+time);
  //duration = parseInt(duration / 60000);
  //var pin = MoodTracker.createPin(time, parseInt(duration / 60000));
  var duration = 3600;
  var index = 0;
  var length = times.length;
  var completed = 0;
  var results = new Array(length);
  var pin = new MoodTracker.Pin(times[i], duration, [times[i].getHours()]);
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
  console.log('Inserting pin in the future: ' + JSON.stringify(pin));
  
  // Insert for today
  pin = pin.insertPin();
  /*insertUserPin(pin, function(responseText) { 
    console.log('Result: ' + responseText);
    
    // Insert for tomorrow
    time.setDate(time.getDate() + 1);
    pin = new MoodTracker.Pin(time, duration).getPin();
    //pin = MoodTracker.createPin(time, parseInt(duration / 60000));
    insertUserPin(pin, function(responseText) { 
      console.log('Result: ' + responseText);
    
      // Insert for day after tomorrow
      time.setDate(time.getDate() + 1);
      pin = new MoodTracker.Pin(time, duration).getPin(); 
      //pin = MoodTracker.createPin(time, parseInt(duration / 60000));
      insertUserPin(pin, function(responseText) { 
        console.log('Result: ' + responseText);
      });
    });
  });*/
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