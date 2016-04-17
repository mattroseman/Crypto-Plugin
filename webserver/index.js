// grab the packages we need
var express = require('express');
var mysql = require("mysql");
var app = express();
var port = process.env.PORT || 8080;
var diff = require('deep-diff').diff;

var bodyParser = require('body-parser');
app.use(bodyParser.json()); // support json encoded bodies
app.use(bodyParser.urlencoded({ extended: true })); // support encoded bodies

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


// ----------return json of new public keys
// compare list being sent to me with list on the server
app.post('/api/server_sync', function(req, res) {
// get array of origin keys
	var rhs = {}, keys = [];
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
		// console.log(lhs);
		// console.log(rhs);
		var differences = diff(lhs, rhs);
		var key_list = [];

		for(i in differences){
			if(differences[i].kind == "A")
				key_list.push(differences[i].item.lhs);
		}
		// now have a list of id's for the missing pub keys
		// get a list of the actual pub keys from DB
		con.query('SELECT pub_key FROM `keys` WHERE id IN (?) ', [ key_list ], function (err, results) {
			var data_results = [];
			if(err) throw err;
			for(i in results){
			data_results.push(results[i].pub_key);
			}
			res.send(data_results);
		});
	});
});
// ---------------fetch a pub key, given a userID
app.post('/api/id_to_pub', function(req, res) {
	console.log(req.headers.userID);
	var nameID = mysql.escape(req.headers.userID);
// 	con.query('SELECT pub_key WHERE id = ?', nameID, function(err,result){
// 		if(err) throw err;
// 		console.log('Pub Key id:', result.pub_key);
// 		res.send();
// 	});	  
});

// -----------works to register a user
app.post('/api/register', function(req, res) {
	// WHERE name = ' + mysql.escape(username),
	var user_data = [];
	var name = mysql.escape(req.headers.user);
	var h_pass = mysql.escape(req.headers.pass);
	var pub_key = req.headers.pub_key;
	console.log(pub_key);
	var sql = 'INSERT INTO keys SET `pub_key` = ?';
	con.query(sql, pub_key, function(err,res){
		if(err) throw err;
		user_data.push({user: name, h_pass: h_pass, key: res.insertId});
		console.log(user_data);
		con.query('INSERT INTO authenticate SET ?', user_data, function(err,res){
			if(err) throw err;

			console.log('Last insert ID:', res.insertId);
		});	  
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

