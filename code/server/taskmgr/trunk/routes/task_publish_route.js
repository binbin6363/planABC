var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
	console.log("show request "+req);
  res.render('task_publish_page', { title: '任务发布' });
});

module.exports = router;
