// Get query variables
function getQueryParam(variable, defaultValue) {
  // Find all URL parameters
  var query = location.search.substring(1);
  var vars = query.split('&');
  for (var i = 0; i < vars.length; i++) {
    var pair = vars[i].split('=');

    // If the query variable parameter is found, decode it to use and return it for use
    if (pair[0] === variable) {
      return decodeURIComponent(pair[1]);
    }
  }
  return defaultValue || false;
}

/**
 * https://stackoverflow.com/a/19572784
 */
function getAndroidVersion(ua) {
  ua = (ua || navigator.userAgent).toLowerCase(); 
  var match = ua.match(/android\s([0-9\.]*)/);
  return match ? match[1] : false;
}

function loadConfigData() {
  var $mood_min = $("#mood_min");
  var $mood_max = $("#mood_max");
  var $mood_step = $("#mood_step");
  var $mood_backup = null;
  var $reminder_days = $("#reminder_days");
  var $reminder_hours = $("#reminder_hours");
  if (localStorage["mood_min"]) {
    $mood_min.val(localStorage["mood_min"])
  }
  if (localStorage["mood_max"]) {
    $mood_max.val(localStorage["mood_max"])
  }
  if (localStorage["mood_step"]) {
    $mood_step.val(localStorage["mood_step"])
  }
  if (localStorage["mood_backup"]) {
    $mood_backup = $("input[name=mood_backup][value=" + localStorage["mood_backup"] + "]");
    $mood_backup.prop('checked', true).trigger("click");
  }
  if (localStorage["reminder_days"]) {
    $reminder_days.val(localStorage["reminder_days"]);
  }
  if (localStorage["reminder_hours"]) {
    $reminder_hours.val(localStorage["reminder_hours"].split(","));
  }
}

function getConfigData() {
  var $mood_min = $("#mood_min");
  var $mood_max = $("#mood_max");
  var $mood_step = $("#mood_step");
  var $mood_backup = $('[name="mood_backup"]:checked');
  var $reminder_days = $("#reminder_days");
  var $reminder_hours = $("#reminder_hours");

  var options = {
    "mood_min": +$mood_min.val(),
    "mood_max": +$mood_max.val(),
    "mood_step": +$mood_step.val(),
    "mood_backup": +$mood_backup.val(),
    "reminder_days": +$reminder_days.val(),
    "reminder_hours": $reminder_hours.val().map(Number)
  };

  // Save for next launch
  localStorage["mood_min"] = options["mood_min"] + "";
  localStorage["mood_max"] = options["mood_max"] + "";
  localStorage["mood_step"] = options["mood_step"] + "";
  localStorage["mood_backup"] = options["mood_backup"] + "";
  localStorage["reminder_days"] = options["reminder_days"] + "";
  localStorage["reminder_hours"] = options["reminder_hours"];

  console.log("Got options: " + JSON.stringify(options));
  return options;
}


$(document).ready(function () {
  var android_version = getAndroidVersion();
  if (android_version !== false && parseFloat(android_version) < 4.4) {
    $("body").addClass("android-fix");
  }
  loadConfigData();

  $("#mood_min, #mood_max").on("change", function(oEvent) {
    var $mood_min = $("#mood_min");
    var $mood_max = $("#mood_max");
    if ($mood_min.val() === $mood_max.val()) {
      switch ($(this).attr("id")) {
        case "mood_min":
          // ensure that mood_max is always greater than mood_min
          $mood_max.get(0).stepUp(1);
          break;
        case "mood_max":
          // ensure that mood_min is always less than mood_max
          $mood_min.get(0).stepDown(1);
          break;
      }
    }
  });

  $("button.decrement, button.increment").on("click", function(oEvent) {
    var $this = $(this);
    var $input = $("#" + $this.data("input-id"));
    if ($this.hasClass("decrement")) {
      $input.get(0).stepDown(1);
    } else {
      $input.get(0).stepUp(1);
    }
    $input.trigger("change");
  });

  $("#submit_button").on("click", function(oEvent) {
    oEvent.preventDefault();
    // Set the return URL depending on the runtime environment
    var return_to = getQueryParam('return_to', 'pebblejs://close#');
    document.location = return_to + encodeURIComponent(JSON.stringify(getConfigData()));
  });
});
