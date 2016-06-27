// grab the packages we need
var express = require('express');
var mysql = require("mysql");
var app = express();
var port = process.env.PORT || 8080;
var diff = require('deep-diff').diff;
var request = require('request'); // make http calls to knurld
var bodyParser = require('body-parser'); // parse responses
app.use(bodyParser.json()); // support json encoded bodies
app.use(bodyParser.urlencoded({ extended: true })); // support encoded bodies

var knurld_oauth = "Bearer ";
var knurld_dev_id = 'Bearer: eyJhbGciOiJIUzUxMiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE1MDQ4MTY5MDUsInJvbGUiOiJhZG1pbiIsImlkIjoiNTU3MWMzYTVjMjAzZjE3ODI2NzQwZTkwMTlhMGVjYTQiLCJ0ZW5hbnQiOiJ0ZW5hbnRfbXJwdGF4M29tejJoaXkzbm5ucXhhMnR2cGJxdGk2dHBuNTNoYzUzeGdyMmhhM3R6cGJ0d3c2dGVveSsrKysrKyIsIm5hbWUiOiJhZG1pbiJ9.8sdpttZ7h7FWVqaaFzhxpwlYCl7I8ePHkXzhSjrvrIYZ4y8g0Fn40riDvCupSxjh6Rq00NXlgc5ikd0fOUQpyw';

// create connection to db
var con = mysql.createConnection({
	host: "localhost",
    user: "root",
    password: "lolipop123",
    database: "ezkeze"

});

// get access token from knurld
request.post({
	url:'https://api.knurld.io/oauth/client_credential/accesstoken?grant_type=client_credentials', 
	form: {
		'client_id':' 0gHhDE2O9So0sBfSdA7ur4cvNI8qV6dm', 
		'client_secret':'piYbA87jmWUKpNbj'
		}
	}, function(err,httpResponse,body){
	if(err){
		console.log('Error in getting OAuth key');
		return;
	}
	knurld_oauth = knurld_oauth + JSON.parse(body).access_token;
	//console.log(knurld_auth);
	  });

// connect to database 
con.connect(function(err){
	if(err){
		console.log('Error connecting to Db');
		console.log(err);
		return;
	}
	console.log('Connection established');
});

// -----------works to register a user
app.post('/api/register', function(req, res) {
	data = [];
	// construct a JSON from the http request
	var user_data = [];
	var email = mysql.escape(req.headers.email);
	var password = mysql.escape(req.headers.password);
	var public_key = req.headers.public_key;
	var knurld_consumer_id;
	// register the consumer to knurld, and get their consumer ID
	request.post({
			url: 'https://api.knurld.io/v1/consumers', 
			headers:  {
				"Authorization" : knurld_oauth,
				"Developer-Id" : knurld_dev_id
			}, 
			json: {
				username: String(email),
				gender: String(req.headers.gender),
				password: String(password)
			}
			}, function(err, response, body){
				if(err) {
					console.log(err);
				} else {
					body = JSON.stringify(body);
				       	knurld_consumer_id = body.substring(44, body.length - 2); 
					console.log(knurld_consumer_id);
				}
				user_data.push({email: email, password: password, public_key: public_key, knurld_id: knurld_consumer_id});
				console.log(user_data);
	// TODO check for duplicate registered username, or client id
				if(con.query('INSERT INTO users SET ?', user_data, function(err,mysql_res){
					if(err) {data.push({data: "fail"}); 
						res.send(JSON.stringify(data));
					} else {
						console.log('Last insert ID:', res.insertId);
						data.push({data: "success"}); 
						res.send(JSON.stringify(data));
					}
				})) console.log("message sent back");	  
			});


});
// TODO get pending invitations

// ----------return json of public keys associated with user id
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
	});
});
// ---------------fetch a pub key, given a userID
app.get('/api/id_to_pub', function(req, res) {
	console.log(req.headers.email);
	var email = mysql.escape(req.headers.email);
	con.query('SELECT * FROM `users` JOIN key_dump ON key_dump.id = users.public_key WHERE `email` = ?', email, function(err, result){
		if(err) throw err;
		console.log(result);
		//console.log("pub key: ?", result.public_key);
		//res.send(result.public_key);
	
	});
});


// -----------returns success if login works, else wrong password
// TODO fix function properly - send response codes, and check email/pass combo
app.post('/api/login', function(req, res) {
	var h_pass = mysql.escape(req.headers.password);
	var h_user = mysql.escape(req.headers.email);
	var query = "SELECT *  FROM users where email = " + h_user + " AND password = " + h_pass;
	con.query(query,  function(err,rows){
		if(err) throw err;
		else{
			if(rows.length > 0) {
				console.log(rows);
				res.send("success");
			}
			else res.send("fail");
		}
	});
});


// -----------put up an encrypted AES key for another user to download
// TODO change to reflect the new database
app.post('/api/invite', function(req, res) {
	// take the key, add record under a persons name
	var user_data = [];
	var name = req.headers.user_id;
	var pub_key = req.headers.pub_key;
	user_data.push({username: name, key: pub_key});
	console.log(user_data);
	con.query('INSERT INTO user_maps SET ?', user_data, function(err,res){
		if(err) throw err;
		console.log('Last insert ID:', res.insertId);
	});	  
});
// start the server
app.listen(port);
console.log('Server started: localhost:'+port); 
// con.end(function(err) {
// 	console.log('mysql con terminated - error?');
// 	// The connection is terminated gracefully
// 	// Ensures all previously enqueued queries are still
// 	// before sending a COM_QUIT packet to the MySQL server.
// });

