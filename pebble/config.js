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

function loadConfigData() {
  var $mood_min = $("#mood_min");
  var $mood_max = $("#mood_max");
  var $mood_step = $("#mood_step");
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
  var $reminder_days = $("#reminder_days");
  var $reminder_hours = $("#reminder_hours");

  var options = {
    "mood_min": +$mood_min.val(),
    "mood_max": +$mood_max.val(),
    "mood_step": +$mood_step.val(),
    "reminder_days": +$reminder_days.val(),
    "reminder_hours": $reminder_hours.val().map(Number)
  };

  // Save for next launch
  localStorage["mood_min"] = options["mood_min"] + "";
  localStorage["mood_max"] = options["mood_max"] + "";
  localStorage["mood_step"] = options["mood_step"] + "";
  localStorage["reminder_days"] = options["reminder_days"] + "";
  localStorage["reminder_hours"] = options["reminder_hours"];

  console.log("Got options: " + JSON.stringify(options));
  return options;
}


$(document).ready(function() {
  loadConfigData();
  $("#submit_button").on("click", function(oEvent) {
    oEvent.preventDefault();
    // Set the return URL depending on the runtime environment
    var return_to = getQueryParam('return_to', 'pebblejs://close#');
    document.location = return_to + encodeURIComponent(JSON.stringify(getConfigData()));
  });
});
