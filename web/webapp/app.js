var express = require('express');
var path = require('path');
var favicon = require('serve-favicon');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');

var index = require('./routes/index');
var tickerApi = require('./routes/ticker');
var mongo = require('mongodb');
var monk = require('monk');
var auth = require("basic-auth");
var fs = require("fs");
var ticker = require("./tickerInterface.js");


// Read Synchronously
var passwdFile = fs.readFileSync("passwdFile.json");
var passwd = JSON.parse(passwdFile);

if((passwd.api.user === undefined)||(passwd.api.password === undefined) ||
  (passwd.db.user === undefined)||(passwd.db.password === undefined)){
  console.log("Bad password file");
}

console.log("Db user: " + passwd.db.user)
console.log("Db password: " + passwd.db.password)
var db = monk(''+passwd.db.user+':'+passwd.db.password+'@localhost:27017/ledTicker');

// var db = monk('localhost:27017/ledTicker',{
//   username : passwd.db.user,
//   password : passwd.db.password
// });
//var io = require('socket.io')(6000);

var app = express();

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'pug');

// uncomment after placing your favicon in /public
//app.use(favicon(path.join(__dirname, 'public', 'favicon.ico')));
app.use(logger('dev'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));
app.use(express.static(__dirname + '/node_modules/bootstrap/dist'));


admins = {}
admins[passwd.api.user] = {"password": passwd.api.password};

console.log("API user: " + passwd.api.user)
console.log("API password: " + passwd.api.password)


app.use(function(request, response, next) {
  var user = auth(request);
  if (!user || !admins[user.name] || admins[user.name].password !== user.pass) {
    response.set('WWW-Authenticate', 'Basic realm="example"');
    return response.status(401).send();
  }
  return next();
});

// Make our db accessible to our router
app.use(function(req,res,next){
    req.db = db;
    next();
});

app.use('/', index);
app.use('/ticker', tickerApi);

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

// error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  console.log(err.stack);
  res.render('error');
});

// Initialize ticker background processes
ticker.init(db); 


module.exports = app;
