var express = require('express');
var router = express.Router();


router.post('/publish', function(req, res, next) {
        console.log("show request "+req.body);
  //res.render('task_publish_page', { title: '任务发布' });
});

module.exports = router;
