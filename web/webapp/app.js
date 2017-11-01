var express = require('express');
var path = require('path');
var favicon = require('serve-favicon');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');

var index = require('./routes/index');
var mongo = require('mongodb');
var monk = require('monk');
var db = monk('localhost:27017/ledTicker');
var io = require('socket.io')(6000);

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

// Make our db accessible to our router
app.use(function(req,res,next){
    req.db = db;
    next();
});

app.use('/', index);

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
  res.render('error');
});

// When we get a connection from socket.io
io.on('connection', function(socket){
	console.log('Somebody connected');
	socket.on('scrollAlert', function(msg){
		console.log("Ticker message has been scrolled off the screen");
		console.log(msg);

		// Validate parameters


		// Has the ticker data been updated since last update?
		if(TickerMessageChanged){
			// Kill process?
			// Create new image
			console.log("launching child process");
			const { spawn } = require('child_process');
			const createImageScript = spawn('/Users/bclouser/workspace/ledTicker/createImage/createImages.sh');

			createImageScript.stdout.on('data', (data) => {
				console.log(`stdout: ${data}`);
			});
			createImageScript.stderr.on('data', (data) => {
				console.log(`stderr: ${data}`);
			});
			createImageScript.on('close', (code) => {
				console.log(`child process exited with code ${code}`);
				if(code != 0){
					console.log("Failed to create image")
					return;
				}

				// Re-Launch ticker app
				// 
				const tickerApp = spawn('/Users/bclouser/workspace/ledTicker/createImage/createImages.sh');
			});
		}
	});
});


module.exports = app;
