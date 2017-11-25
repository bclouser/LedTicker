var zmq = require('zeromq')
var sock = zmq.socket('pub');
var snow = require("./onTheSnow.js");

// publish
sock.bindSync("tcp://*:5556");
console.log('Publisher bound to port 5556');

// These two need to be globals
var database;
var publish;

/*
Inside the database things look like this:
{
	"dailyProcessed": "",
	"monthlyProcessed": "",
	"totalProcessed": "",
	"numRestaurants": "",
	"railsInField": "",
	"beerPoured": "",
	"announcement": "",
	"mountainData":{
					...
					}
}
*/

exports.publishToTicker = function(tickerData){
    var monthName = new Array();
    monthName[0] = "January";
    monthName[1] = "February";
    monthName[2] = "March";
    monthName[3] = "April";
    monthName[4] = "May";
    monthName[5] = "June";
    monthName[6] = "July";
    monthName[7] = "August";
    monthName[8] = "September";
    monthName[9] = "October";
    monthName[10] = "November";
    monthName[11] = "December";

    var dayOfWeek = new Array();
    dayOfWeek[0] = "Sun";
    dayOfWeek[1] = "Mon";
    dayOfWeek[2] = "Tues";
    dayOfWeek[3] = "Wed";
    dayOfWeek[4] = "Thurs";
    dayOfWeek[5] = "Fri";
    dayOfWeek[6] = "Sat";
    
    var date = new Date();
    var yesterday = new Date();
    yesterday.setDate(date.getDate() - 1);
    var yesterdayName = dayOfWeek[yesterday.getDay()];
    var lastMonth = new Date();
    lastMonth.setMonth(date.getMonth() - 1);
    lastMonthName = monthName[lastMonth.getMonth()];

    var dailyProcessed = tickerData["dailyProcessed"];
    var monthlyProcessed = tickerData["monthlyProcessed"];
    var totalProcessed = tickerData["totalProcessed"];
    var numRestaurants = tickerData["numRestaurants"];
    var railsInField = tickerData["railsInField"];
    var beerPoured = tickerData["beerPoured"];
    var announcement = tickerData["announcement"];

    if(dailyProcessed==null){ dailyProcessed = 0;};
    if(monthlyProcessed==null){ monthlyProcessed = 0;}
    if(totalProcessed==null){ totalProcessed = 0;}
    if(numRestaurants==null){ numRestaurants = 0;}
    if(railsInField==null){ railsInField = 0;}
    if(beerPoured==null){ beerPoured = 0;}

    if(tickerData["mountainData"]){
	    // Ski Mountain Stuff
		stevensSnowAccum = tickerData["mountainData"]["stevens-pass-resort"].snowTodayInches;
		stevensTemp = tickerData["mountainData"]["stevens-pass-resort"].temperatureBase;
		stevensWeather = tickerData["mountainData"]["stevens-pass-resort"].weatherBase;
		crystalSnowAccum = tickerData["mountainData"]["crystal-mountain-wa"].snowTodayInches;
		crystalTemp = tickerData["mountainData"]["crystal-mountain-wa"].temperatureBase;
		crystalWeather = tickerData["mountainData"]["crystal-mountain-wa"].weatherBase;
		mtBakerSnowAccum = tickerData["mountainData"]["mt-baker"].snowTodayInches;
		mtBakerTemp = tickerData["mountainData"]["mt-baker"].temperatureBase;
		mtBakerWeather = tickerData["mountainData"]["mt-baker"].weatherBase;
		snoqualmieSnowAccum = tickerData["mountainData"]["the-summit-at-snoqualmie"].snowTodayInches;
		snoqualmieTemp = tickerData["mountainData"]["the-summit-at-snoqualmie"].temperatureBase;
		snoqualmieWeather = tickerData["mountainData"]["the-summit-at-snoqualmie"].weatherBase;
	}
    

    var tickerString = "&c14552F:money::moneybag::money::moneybag: Yesterday("+yesterdayName+"): $"+dailyProcessed.toFixed(2) +", In "+lastMonthName+": $"+monthlyProcessed.toFixed(2)+", To Date: $"+totalProcessed.toFixed(2)+", &c14552FRestaurants: "+numRestaurants+", Deployed Rails: "+railsInField+", &cD2A9E3:beer::beer::beer::beer::beer::beer: Keg Poured: "+beerPoured+" gallons. "+announcement+ " :snowflake::snowflake:"+
    	" Crystal: "+crystalWeather+", "+crystalTemp+", snowfall "+crystalSnowAccum+"\"." +
    	" Stevens Pass: "+stevensWeather+", "+stevensTemp+", snowfall "+stevensSnowAccum+"\"." +
    	" Snoqualmie: "+snoqualmieWeather+", "+snoqualmieTemp+", snowfall "+snoqualmieSnowAccum+"\"." +
    	" Mt Baker: "+mtBakerWeather+", "+mtBakerTemp+", snowfall "+mtBakerSnowAccum+"\""

    console.log("Sending message to ticker: \n" + tickerString + "\n");

    sock.send(['tickerData', tickerString]);
}

var onMountainData = function(err, inMtnData){
	if(err){
		console.log("Failed to get mountain data from onTheSnow. error:");
		console.log(err);
	}
	else{
		console.log("Mountains Data Retrieved successfully, Updating database");
		var mtnNames = snow.washingtonMtnNames;
		var mtnData = {};
		for(var i in mtnNames){
			if(mtnNames[i] in inMtnData){
				// Only use data for mountains that we know about
				mtnData[mtnNames[i]] = inMtnData[mtnNames[i]];
			}
		}
		var collection = database.get('tickerData');
		collection.findOne({"name" : "tablesafe"},{},function(err,doc){
			if(err){
				console.log(err.stack);
				res.json({"success":false, "error": err.stack});
			}
		    doc["mountainData"] = mtnData;
		    collection.update(
			{"name" : "tablesafe"},
			doc,
			{"upsert":true},
			function (err, doc) {
				if (err) {
					console.log("There was an error updating Mountain data in the db");
					console.log(err.stack);
					return;
				}
				// Update ticker with the latest stuff.
				publish(doc);
			});
		});
	}
}

//// Set and start an interval for how often the ticker will be updated
exports.init = function(db, interval_seconds){
	// These two need to be globals
	database = db;
	publish = this.publishToTicker;
	console.log("Kickoff Ski Mountain data retrieval");
	// call it once first to get things updated initially.
	snow.getAllWashingtonMtns(onMountainData);
	// Once every two hours we should update the database with the latest snow info
	setInterval(function(){
		snow.getAllWashingtonMtns(onMountainData);
	//}, 2* 60 * 60 * 1000);
	}, 60 * 1000);

	// Send out a ticker update right now with whatever is in the db
    var collection = database.get('tickerData');
    if(collection != null){
    	collection.findOne({"name" : "tablesafe"},{},function(err,doc){
    		if(err){
    			console.log("Failed to find document in database");
    			console.log(err.stack);
    		}
    		publish(doc);
    	});
    }

	console.log("Setting Ticker update Interval to be "+interval_seconds+" seconds");
	// Default is two hours
	if(interval_seconds == null){
		interval_seconds = 2 * 60 * 60;
	}
	setInterval(function(){
		var collection = database.get('tickerData');
		if(collection != null){
			collection.findOne({"name" : "tablesafe"},{},function(err,doc){
				if(err){
					console.log("Failed to find document in database");
					console.log(err.stack);
				}
				publish(doc);
			});
		}
	}, interval_seconds* 1000); 

	console.log("Ticker Interface initialized");
}
