
$(document).ready(function() {

  $("#submit_button").on("click", function(oEvent) {
    oEvent.preventDefault();
    var config = {
      "mood_min": +$("#mood_min").val(),
      "mood_max": +$("#mood_max").val(),
      "mood_step": +$("#mood_step").val()
    };
    console.log(config);
  });
});
