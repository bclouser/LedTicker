var express = require('express');
var router = express.Router();
var bodyParser = require('body-parser');
var ticker = require("../tickerInterface.js");

// create application/json parser
var jsonParser = bodyParser.json();

function validateAndParseParam(key, value){
    var validParams = {
        "dailyProcessed":"float",
        "monthlyProcessed":"float",
        "totalProcessed":"float",
        "railsInField":"int",
        "numRestaurants":"int",
        "announcement":"string"
    };

    console.log("key: " + key + " value: " + value);

    if(key in validParams){
        if(validParams[key] == "float"){
            return parseFloat(value);
        }
        else if(validParams[key] == "int"){
            return parseInt(value);
        }
        else if(validParams[key] == "bool"){
            if(typeof value == string){
                return value == "true";
            }
        }
        else{
            return value;
        }
    }
    else{
        console.log("Ok, key isn't in validparams");
    }
    return null;
}

/* Request to update individual field */
router.post('/update/:fieldName&:fieldValue', jsonParser, function(req, res) {

    var key = req.params.fieldName;
    var value = req.params.fieldValue;

    value = validateAndParseParam(key, value);

    if(!value){
        return res.json({"success":false, "error":"Bad api endpoint. No matching field for " + key + " or invalid param"});
    }

    var db = req.db;
    var collection = db.get('tickerData');
    collection.findOne({"name" : "tablesafe"},{},function(err,doc){
        if(err){
            console.log(err.stack);
            res.json({"success":false, "error": err.stack});
        }

        console.log("name: " + key + " value: "+ value);
        doc[key] = value;
        collection.update(
            {"name" : "tablesafe"},
            doc,
            {"upsert":true},
            function (err, doc) {
                if (err) {
                    console.log("There was an error updating "+req.params.dataField+" in the db");
                    console.log(err.stack);
                    // If it failed, return error
                    //res.send("There was a problem adding the latest ticker data to the database.");
                    res.json({"success":false,"error": err.stack});
                }
                else{
                    // Build and publish the tickerString 
                    ticker.publishToTicker(doc);
                    // Success
                    res.json({"success":true});
                }
            }
        );
    });
});

/* Post to get tickerData from the db */
router.post('/data', jsonParser, function(req, res) {
    var db = req.db;
    var collection = db.get('tickerData');
    collection.findOne({"name" : "tablesafe"},{},function(err,doc){
        if(err){
            console.log(err.stack)
            res.json({"error":err.stack});
        }
        else{
            res.json(doc);
        }
    });
});

/* POST request to update ticker data */
router.post('/update', function(req, res) {

    var ret = "";
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
            res.render('error', err.stack);
        }
        if(activeDoc == null){
            console.log("Database doesn't contain any documents yet...");
            activeDoc = {"name":"tablesafe"};
            // This doesn't totally solve the issue.
            // TODO: Actually handle things when our database is empty
        }

        for(var key in req.body){
            // We only care about the params they did pass in
            if(!req.body[key]){
                continue;
            }
            var value = validateAndParseParam(key, req.body[key]);
            if(value){
                activeDoc[key] = value;
            }
            else{
                ret += "invalid parameter: " + key +"\n";
            }
        }

        if(ret){
            console.log(ret);
            return res.render('error', ret);
        }

        // Build and publish the tickerString 
        ticker.publishToTicker(activeDoc);

        collection.update(
            {"name" : "tablesafe"},
            activeDoc,
            {"upsert":true}, // Replace existing fields with these values
            function (err, doc) {
                if (err) {
                    console.log("There was an error updating the tickerData in the db");
                    console.log(err.stack);
                    // If it failed, return error
                    res.render('error',err.stack);
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