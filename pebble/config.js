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
  if (localStorage["mood_min"]) {
    $mood_min.val(localStorage["mood_min"])
  }
  if (localStorage["mood_max"]) {
    $mood_max.val(localStorage["mood_max"])
  }
  if (localStorage["mood_step"]) {
    $mood_step.val(localStorage["mood_step"])
  }
}

function getConfigData() {
  var $mood_min = $("#mood_min");
  var $mood_max = $("#mood_max");
  var $mood_step = $("#mood_step");

  var options = {
    "mood_min": +$mood_min.val(),
    "mood_max": +$mood_max.val(),
    "mood_step": +$mood_step.val()
  };

  // Save for next launch
  localStorage["mood_min"] = options["mood_min"] + "";
  localStorage["mood_max"] = options["mood_max"] + "";
  localStorage["mood_step"] = options["mood_step"] + "";

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
