var express = require('express');
var router = express.Router();

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

router.get('/tickerData', function(req, res) {
    var db = req.db;
    var collection = db.get('tickerData');
    collection.find({},{},function(e,docs){
        res.render('tickerData', docs);
    });
});

/* POST to Add User Service */
router.post('/ticker', function(req, res) {

    // Set our internal DB variable
    var db = req.db;

    // Get our form values. These rely on the "name" attributes in html form
    var dailyProcessed = parseFloat(req.body.dailyProcessed);
    var monthlyProcessed = parseFloat(req.body.monthlyProcessed);
    var railsInField = parseInt(req.body.railsInField);
    var numRestaurants = parseInt(req.body.numRestaurants);

    console.log("dailyProcessed " + typeof dailyProcessed);
    console.log("monthlyProcessed " + typeof monthlyProcessed);
    console.log("railsInField " + typeof railsInField);
    console.log("numRestaurants " + typeof numRestaurants);


    // TODO error checking and check against empty

    // Set our collection
    var collection = db.get('tickerData');

    console.log("Updating the database");
    // Submit to the DB
    collection.update(
        {"name" : "tablesafe"},
        {
        "name" : "tablesafe",
        "dailyProcessed" : dailyProcessed,
        "monthlyProcessed" : monthlyProcessed,
        "railsInField" : railsInField,
        "numRestaurants" : numRestaurants
        },
        {"upsert":true},
        function (err, doc) {
            if (err) {
                console.log("Ok, so there was an error updating the tickerData in the db");
                // If it failed, return error
                //res.send("There was a problem adding the latest ticker data to the database.");
                res.redirect('error',err.stack);
            }
            else {
                console.log("Looks like the db update worked")
                // And forward to success page
                res.redirect('/');
            }
        }
    );
});

module.exports = router;
