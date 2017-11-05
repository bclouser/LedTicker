var express = require('express');
var router = express.Router();
var bodyParser = require('body-parser');
var zmq = require('zeromq')
var sock = zmq.socket('pub');
// pubber.js
sock.bindSync("tcp://*:5556");
console.log('Publisher bound to port 5556');

// create application/json parser
var jsonParser = bodyParser.json()



/* GET home page. */
router.get('/', function(req, res, next) {
    res.render('index', { title: 'Express' });
});

/* GET Userlist page. */
/*
router.get('/userlist', function(req, res) {
    var db = req.db;
    var collection = db.get('usercollection');
    collection.find({},{},function(e,docs){
        res.render('userlist', {
            "userlist" : docs
        });
    });
});
*/

router.post('/tickerData', jsonParser, function(req, res) {
    var db = req.db;
    var collection = db.get('tickerData');
    collection.findOne({"name" : "tablesafe"},{},function(err,doc){
        if(err){
            console.log(err.stack)
            res.json({"error":err.stack});
        }
        res.json(doc);
    });
});

function publishToTicker(tickerString){
    sock.send(['tickerData', tickerString]);
    //sock.send(doc);
}

/* POST to Add User Service */
router.post('/ticker', function(req, res) {

    // Set our internal DB variable
    var db = req.db;
    // Get our form values. These rely on the "name" attributes in html form
    var dailyProcessed = parseFloat(req.body.dailyProcessed)
    var monthlyProcessed = parseFloat(req.body.monthlyProcessed)
    var totalProcessed = parseFloat(req.body.totalProcessed)
    var railsInField = parseInt(req.body.railsInField);
    var numRestaurants = parseInt(req.body.numRestaurants);

    // get our collection
    var collection = db.get('tickerData');

    collection.findOne({"name" : "tablesafe"},{},function(err,activeDoc){
        if(err){
            console.log(err.stack);
            return;
        }
        
        if(!dailyProcessed){
            dailyProcessed = activeDoc.dailyProcessed;
        }
        if(!monthlyProcessed){
            monthlyProcessed = activeDoc.monthlyProcessed;
        }
        if(!totalProcessed){
            totalProcessed = activeDoc.totalProcessed;
        }
        if(!railsInField){
            railsInField = activeDoc.railsInField;
        }
        if(!numRestaurants){
            numRestaurants = activeDoc.numRestaurants;
        }
        // Document that will be pushed to the database
        var newDoc = {"name":"tablesafe",
                      "dailyProcessed": dailyProcessed,
                      "monthlyProcessed": monthlyProcessed,
                      "totalProcessed": totalProcessed,
                      "railsInField": railsInField,
                      "numRestaurants": numRestaurants
                    };

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
        console.log()
        var yesterday = new Date();
        yesterday.setDate(date.getDate() - 1);
        var yesterdayName = dayOfWeek[yesterday.getDay()];
        var lastMonth = new Date();
        lastMonth.setMonth(date.getMonth() - 1);
        lastMonthName = monthName[lastMonth.getMonth()];
        var beerPoured = 0;

        var tickerString = "&c14552F&eMONY&eMONY&eMONY&eMONY Yesterday("+yesterdayName+"): $"+dailyProcessed.toFixed(2) +", In "+lastMonthName+": $"+monthlyProcessed.toFixed(2)+", To Date: $"+totalProcessed.toFixed(2)+", &c14552FRestaurants: "+numRestaurants+", Deployed Rails: "+railsInField+", &cD2A9E3&eBEER&eBEER&eBEER&eBEER&eBEER&eBEER Keg Poured: "+beerPoured+" gallons. &c544704&eBENC Slip me some &eRANR&eRANR&eRANR&eRANR!!!";
        console.log("TICKERSTRING: " + tickerString);
        console.log("publishing message out to the ticker");
        publishToTicker(tickerString);

        collection.update(
            {"name" : "tablesafe"},
            newDoc,
            {"upsert":true}, // Replace existing fields with these values
            function (err, doc) {
                if (err) {
                    console.log("There was an error updating the tickerData in the db");
                    console.log(err.stack);
                    // If it failed, return error
                    //res.send("There was a problem adding the latest ticker data to the database.");
                    res.redirect('error',err.stack);
                }
                else {
                    // And forward to success page
                    res.redirect('/');
                }
            }
        );
    });
});

module.exports = router;
