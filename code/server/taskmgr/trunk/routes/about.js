var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('about_page', { title: '任务系统' });
});

module.exports = router;
