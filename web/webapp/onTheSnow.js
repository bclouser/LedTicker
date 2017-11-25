var https = require('https');
var cheerio = require('cheerio');

exports.washingtonMtnNames = [
	"crystal-mountain-wa",
	"stevens-pass-resort",
	"the-summit-at-snoqualmie",
	"mt-baker"
];

exports.getMountainStats = function(mountainName, callback){
	var url = 'https://www.onthesnow.com/washington/'+mountainName+'/skireport.html'
	https.get(url, (res) => {
		//console.log('statusCode:', res.statusCode);
		//console.log('headers:', res.headers);
		var webPage = '';
		var snowToday = ''
		res.on('data', function (chunk) {
			webPage += chunk;
	    });
		res.on('end', function () {
		    var $ = cheerio.load(webPage);
		    // Strip special chars and convert to number
		    var snowTodayInches = Number($('.sr_snowfall_days li.today div.predicted_snowfall div').text().replace(/[^a-zA-Z0-9]/g,''));
		    var status = $("div#snow_conditions li span.current_status").text();
		    data = {
		    	"name":mountainName,
		    	"snowTodayInches":snowTodayInches,
		    	"temperatureSummit":$('li.station.summit div.weather div.temp').text(),
		    	"temperatureBase":$('li.station.base div.weather div.temp').text(),
		    	"weatherSummit":$("li.station.summit div.weather div.icon").attr("class").split(" ")[1],
		    	"weatherBase":$("li.station.base div.weather div.icon").attr("class").split(" ")[1],
		    	"open":(status=="Open"),
		    };
		    callback('', data);
		});
		res.on('error', (e) => {
			callback(e, {});
		});
	});
};

exports.getAllWashingtonMtns = function(callback){
	var mtnNames = this.washingtonMtnNames;
	for(var i in mtnNames){
		var numMtnsProcessed = 0;
		var mountainData = {};
		this.getMountainStats(mtnNames[i], function(err, data){
			if(err){
				console.log("Failed to get data for " + mtnNames[i] );
				callback(err, {});
				return;
			}
			mountainData[data.name] = data;
			// Once we reach the end of the list, callback
			if(numMtnsProcessed == (mtnNames.length-1)){
				callback('', mountainData);
			}
			numMtnsProcessed += 1;
		});
	}
}