
var mqtt    = require('mqtt');
var mqttClient  = mqtt.connect('mqtt://m13.cloudmqtt.com:15840',{
  username: 'vesi',
  password: 'vesimittaukset'
});

var MongoClient = require('mongodb').MongoClient;
 
var myCollection;
var db;
var pl;


mqttClient.on('message', function(topic, message) {
  console.log(message.toString('utf8'));
  pl = message.toString('utf8');

	createConnection(function(){
    		addDocument(function(){
		});
	});

});

mqttClient.on('connect', function(){
    console.log('Connected');
});

mqttClient.subscribe('mittaukset/#');
 
function addDocument(onAdded){
    myCollection.insert({Time: timeConverter(Date.now()), payload: pl}, function(err, result) {
        if(err)
            throw err;
 
        console.log("entry saved");
        onAdded();
    });
}
 
function createConnection(onCreate){
    MongoClient.connect('mongodb://127.0.0.1:27017/measurements2', function(err, db) {
        if(err)
            throw err;
        console.log("connected to the mongoDB !");
        myCollection = db.collection('test_collection');
 
        onCreate();
		db.close();
    });
}
 
if (!Date.now) {
    Date.now = function() { return new Date().getTime(); }
}

function timeConverter(UNIX_timestamp){
  var a = new Date(UNIX_timestamp);
  var months = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
  var year = a.getFullYear();
  var month = months[a.getMonth()];
  var date = a.getDate();
  var hour = a.getHours();
  var min = a.getMinutes();
  var sec = a.getSeconds();
  var time = date + ' ' + month + ' ' + year + ' ' + hour + ':' + min + ':' + sec ;
  return time;
}


//console.log(timeConverter(Date.now()));