var express = require('express');
var router = express.Router();

/* GET users listing. */
router.get('/', function(req, res, next) {
  res.send('i am bbwang. welcome to my page.');
});
module.exports = router;
