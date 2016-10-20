var express = require('express');
var path = require('path');
var favicon = require('serve-favicon');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');

var routes = require('./routes/index');
var users = require('./routes/users');
var showme = require('./routes/showme');
//var task_publish_route = require('./routes/task_publish_route');
var about = require('./routes/about');
var task = require('./routes/task');
var setting = require('./routes/setting');
var verify = require('./routes/verify');
var options = require('./etc/options');

var log4js = require('./etc/logger').log4js;
var logger = require('./etc/logger').logger;

var app = express();
app.use(log4js.connectLogger(logger, {level: 'auto', format:':method :url'}));

var init_network = function (argument) {
//  init(argument);
  //task.init(argument);
}


// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');

// uncomment after placing your favicon in /public
//app.use(favicon(path.join(__dirname, 'public', 'favicon.ico')));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', routes);
app.use('/users', users);
app.use('/showme', showme);
//app.use('/task_publish', task_publish_route);
app.use('/about', about);
//app.use('/task', task);

app.use('/task', task);
app.use('/setting', setting);
app.use('/verify', verify);

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

// error handlers

// development error handler
// will print stacktrace
if (app.get('env') === 'development') {
  app.use(function(err, req, res, next) {
    res.status(err.status || 500);
    res.render('error', {
      message: err.message,
      error: err
    });
  });
}

// production error handler
// no stacktraces leaked to user
app.use(function(err, req, res, next) {
  res.status(err.status || 500);
  res.render('error', {
    message: err.message,
    error: {}
  });
});




module.exports = app;
