$(function() {
	$.post( "/tickerData", {}, 
		function( data ) {
			console.log("Got data back");
			console.log(data);

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
	});
});