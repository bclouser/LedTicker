$(function() {
	$.post( "/ticker/data", {}, 
		function(data) {
			if(!data){
				console.log("Failed to get any data back from server");
				return;
			}
			if(data.hasOwnProperty("dailyProcessed")){
				$("div.form-group #dailyProcessed").attr('placeholder', data.dailyProcessed.toFixed(2));
			}
			if(data.hasOwnProperty("monthlyProcessed")){
				$("div.form-group #monthlyProcessed").attr('placeholder', data.monthlyProcessed.toFixed(2));
			}
			if(data.hasOwnProperty("totalProcessed")){
				$("div.form-group #totalProcessed").attr('placeholder', data.totalProcessed.toFixed(2));
			}
			if(data.hasOwnProperty("railsInField")){
				$("div.form-group #railsInField").attr('placeholder', data.railsInField);
			}
			if(data.hasOwnProperty("numRestaurants")){
				$("div.form-group #numRestaurants").attr('placeholder', data.numRestaurants);
			}
			if(data.hasOwnProperty("announcement")){
				$("div.form-group #announcement").attr('placeholder', data.announcement);
			}
	});

	$('#announcement').jemoji({
		extension:    'png',
		folder:       'jemoji/emojis/',
		btn:    $('#emoji-btn'),
		theme:    'red',
		container:  $('#announcement').parent().parent()
	});

	console.log($('#announcement').jemoji('options'));

	//console.log("Is it happening");
});