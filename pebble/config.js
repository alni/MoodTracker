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


$(document).ready(function() {

  $("#submit_button").on("click", function(oEvent) {
    oEvent.preventDefault();
    var config = {
      "mood_min": +$("#mood_min").val(),
      "mood_max": +$("#mood_max").val(),
      "mood_step": +$("#mood_step").val()
    };
    console.log(config);
    // Set the return URL depending on the runtime environment
    var return_to = getQueryParam('return_to', 'pebblejs://close#');
    document.location = return_to + encodeURIComponent(JSON.stringify(config));
  });
});
