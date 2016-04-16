// grab the packages we need
var express = require('express');
var mysql = require("mysql");
var app = express();
var port = process.env.PORT || 8080;
var diff = require('deep-diff').diff;

// First you need to create a connection to the db
var con = mysql.createConnection({
	host: "localhost",
    user: "root",
    password: "admin",
    database: "chicagohack"

});

con.connect(function(err){
	if(err){
		console.log('Error connecting to Db');
		return;
	}
	console.log('Connection established');
});

var bodyParser = require('body-parser');
app.use(bodyParser.json()); // support json encoded bodies
app.use(bodyParser.urlencoded({ extended: true })); // support encoded bodies

var rhs = {
	"keys": [
		'1',
	'2',
	'3' 
		],
	"user_id": "1"
}

// ----------return json of new public keys
// compare list being sent to me with list on the server
app.post('/api/server_sync', function(req, res) {
// get array of origin keys
	var rhs = {}, keys = [];
	console.log(req.headers);
	keys = JSON.parse(req.headers.keys);
	rhs.keys = keys;

// console.log("KEYS  " + rhs.keys);
	var user = req.headers.user_id;
	rhs.user_id = user;
	
// now get list of keys where headers.user
	var lhs = {}, lhs_keys = [];
	lhs.keys = lhs_keys;
	lhs.user_id = user;	
		
	
	con.query('SELECT `key` FROM `user_maps` WHERE username = ' + user, function(err,rows){
		if(err) throw err;
		for(i in rows)
	{
		lhs.keys.push(rows[i].key);
	}
		console.log(lhs);
		console.log(rhs);
		var differences = diff(lhs, rhs);
		console.log(differences);

	});
});



// -----------works to register a user
app.post('/api/register', function(req, res) {
	// WHERE name = ' + mysql.escape(username),
	var user_data = [];
	var name = mysql.escape(req.headers.user);
	var h_pass = mysql.escape(req.headers.pass);
	user_data.push({user: name, h_pass: h_pass});
	console.log(user_data);
	con.query('INSERT INTO authenticate SET ?', user_data, function(err,res){
		if(err) throw err;

		console.log('Last insert ID:', res.insertId);
		res.send('success');
	});	  
});

// -----------returns success if login works, else wrong password
// not a comprehensive authentication solution
app.post('/api/login', function(req, res) {
	var h_pass = mysql.escape(req.headers.pass);
	con.query('SELECT COUNT(*) AS pass FROM `authenticate` WHERE h_pass = ' + h_pass,  function(err,rows){
		if(err) throw err;
		console.log(rows[0].pass);
		if(rows[0].pass == '0')
	{
		res.send('wrong password');
		return;
	} else
		res.send('success');
	});
});


// start the server
app.listen(port);
console.log('Server started!'); 
// con.end(function(err) {
// 	console.log('mysql con terminated - error?');
// 	// The connection is terminated gracefully
// 	// Ensures all previously enqueued queries are still
// 	// before sending a COM_QUIT packet to the MySQL server.
// });

