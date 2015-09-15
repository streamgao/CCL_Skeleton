var express = require('express');
var app = express();

app.set('port', (process.env.PORT || 5000));
app.use(express.static(__dirname + '/public'));

app.get('/',function(request,response){
	response.send("for portrait, go to /portrait");
});

app.get('/portrait', function(request, response) {
  response.render('portrait');
});

app.listen(app.get('port'), function() {
  console.log("Node app is running at localhost:" + app.get('port'))
});
