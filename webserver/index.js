// grab the packages we need
var express = require('express');
var mysql = require("mysql");
var app = express();
var port = process.env.PORT || 8080;
var diff = require('deep-diff').diff;
var request = require('request'); // make http calls to knurld
var tesseract = require('node-tesseract'); // OCR for image recognition - mobile
var path = require('path');
var multiparty = require('multiparty');
var util = require('util');
var cors = require('cors');
var bodyParser = require('body-parser');
var fs = require('fs');
var suspend = require('suspend');
var sleep = require('sleep');

app.use(cors());
app.use(bodyParser.json({limit: '50mb'}));
app.use(bodyParser.urlencoded({limit: '50mb', extended: true}));

app.post("/api/blobs", function(req, res){
	console.log(req.body.blob);
	var buf = new Buffer(req.body.blob, 'base64'); // decode
	console.log("CREATED BUFFER");
	var filename = '../../html/uploads/' + req.body.email + req.body.time_stamp + '.wav';
	fs.writeFile(filename, buf, function(err) {
		if(err) {
			console.log("err", err);
		} else {
			return res.json({'status': 'success'});
		}
	});
});

// do not modify
var knurld_enrollment_id = "";
var knurld_app_model_id = "ecd1003f382e5a3f544d2f1dcf97bc93";
var knurld_oauth = "Bearer ";
var knurld_dev_id = 'Bearer: eyJhbGciOiJIUzUxMiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE1MDQ4MTY5MDUsInJvbGUiOiJhZG1pbiIsImlkIjoiNTU3MWMzYTVjMjAzZjE3ODI2NzQwZTkwMTlhMGVjYTQiLCJ0ZW5hbnQiOiJ0ZW5hbnRfbXJwdGF4M29tejJoaXkzbm5ucXhhMnR2cGJxdGk2dHBuNTNoYzUzeGdyMmhhM3R6cGJ0d3c2dGVveSsrKysrKyIsIm5hbWUiOiJhZG1pbiJ9.8sdpttZ7h7FWVqaaFzhxpwlYCl7I8ePHkXzhSjrvrIYZ4y8g0Fn40riDvCupSxjh6Rq00NXlgc5ikd0fOUQpyw';

// create connection to db
var con = mysql.createConnection({
	host: "localhost",
	user: "root",
	password: "lolipop123",
	database: "ezkeze"

});

//function1() {
//console.log("hello, run this right away");
//}
//function2( {
//console.log("run this every 2 seconds");
//}
//function3() {

//setTimeout(function2, 3000);
//function3();
//}
//function3();
//function1();
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

// -----------register a user with us and knurld, and enroll them to the app model
app.post('/api/register', function(req, res) {
	data = [];
	// construct a JSON from the http request
	var user_data = [];
	var email = req.body.email;
	var password = req.body.password;
	var public_key = req.body.public_key;
	var knurld_consumer_id;
	var audio_blob = req.body.blob;
	var blob_name = req.body.blob_name;
	console.log(email);
	// register the consumer to knurld, and get their consumer ID
	if(!email || !password || !public_key  === 'undefined')
		res.send("a field you sent is null");
	request.post({
		url: 'https://api.knurld.io/v1/consumers',
			headers:  {
				"Authorization" : knurld_oauth,
				"Developer-Id" : knurld_dev_id
			},
		json: {
			username: email,
			gender: req.body.gender,
			password: password
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

		if(con.query('INSERT INTO users SET ?', user_data, function(err,mysql_res){
			if(err) {data.push({data: "fail"});
				console.log(err);
				res.send(JSON.stringify(data));
			} else {
				console.log('Last insert ID:', res.insertId);
				request.post({
					url: 'https://api.knurld.io/v1/enrollments',
					headers: {
						"Authorization": knurld_oauth,
						"Developer-Id": knurld_dev_id
						},
					json: {
						"consumer": "https://api.knurld.io/v1/consumers/" + knurld_consumer_id,
						"application": "https://api.knurld.io/v1/app-models/" + knurld_app_model_id
					}
				}, function(err, response, body){
					if(err) {
						console.log(err + ' -- error in enrolling someone to the app model');
					} else {
				data.push({data: "success"});
				res.send(JSON.stringify(data));
				console.log("their response: ");
						console.log("Enrollment ID: " + body.href);
						knurld_enrollment_id = body.href.substring(37, body.href.length);
						console.log("ENROLLMENT ID", knurld_enrollment_id);
						console.log("OAUTH TOKEN", knurld_oauth);
						// step 5b, record enrollment audio in the user's voice
						//var buf = new Buffer(req.body.blob, 'base64'); // decode
						//console.log("CREATED BUFFER");
						//var filename = '/var/www/html/uploads/' + req.body.blob_name + '.wav';
						//fs.writeFile(filename, buf, function(err) {
							//if(err) {
								//console.log("err", err);
							//} else {
								//console.log("successfully saved file to public folder - /var/www/html/uploads" + blob_name);
							//}
							 //move all code in here for asynch
						//});
						// save the blob file to folder
						// step 5c, extract word endpoints in enrollment audio file
						blob_name = '420blaiseit@gmail.com1467251134197';
						request.post({
							url: 'https://api.knurld.io/v1/endpointAnalysis/url',
							headers: {
								"Authorization": knurld_oauth
								},
							json: {
								"audioUrl": "http://stoh.io/uploads/" + blob_name +  '.wav'
							}
						}, function(err, response, body){
							if(err) {
								console.log(err + ' -- error in step 5c - extract word endpoints in enrollment audio file');

							} else {
								console.log(body);
								console.log(body.taskName + ' one more print statment left');
								sleep.sleep(3);
								// second part of task 6d, extract work endpoints in enrollment audio file
								request.get('https://api.knurld.io/v1/endpointAnalysis/' + body.taskName,
										 {
											 headers:
											 {
												   "Authorization": knurld_oauth
											   }
										 }, function (error, response, body) {
											var taskStatus = JSON.parse(body).taskStatus;
											var taskBody = JSON.parse(body); 
											console.log(taskStatus);
										// perform task 6e, complelete enrollment work order	 
										taskBody.intervals[0]['phrase'] = "Cleveland";
										taskBody.intervals[1]['phrase'] = "Cleveland";
										taskBody.intervals[2]['phrase'] = "Cleveland";
										taskBody.intervals[3]['phrase'] = "Denver";
										taskBody.intervals[4]['phrase'] = "Denver";
										taskBody.intervals[5]['phrase'] = "Denver";
										taskBody.intervals[6]['phrase'] = "Triangle";
										taskBody.intervals[7]['phrase'] = "Triangle";
										taskBody.intervals[8]['phrase'] = "Triangle";

										intervals = taskBody.intervals;
										console.log(intervals);
										request.post({
											url: 'https://api.knurld.io/v1/enrollments/' + knurld_enrollment_id,
											headers: {
												"Authorization": knurld_oauth,
												"Developer-Id": knurld_dev_id
												},
											json: {
												"verification.wav": "http://stoh.io/uploads/" + blob_name + '.wav',
												"intervals": intervals

											}
										}, function(err, response, body){
											if(err) {
												console.log(err + ' -- error in 5d');

											} else {
												console.log(body);
												console.log("http://stoh.io/uploads/" + blob_name + '.wav');
												//knurld_enrollment_id = body.href.substring(37, body.href.length);
												//console.log(knurld_enrollment_id);
												// if interested, perform task 5e - get enrollment completion status
												//sleep.sleep(3);


											
											}
										});



											 
										 });
							}
					});
				}
			});
		} console.log("message sent back");
	}));
});
});

// test endpoint for uploading an image
app.get('/uploadImage', function (req, res) {
	res.writeHead(200, {'content-type': 'text/html'});
	res.end(
			'<form action="/api/get_text" enctype="multipart/form-data" method="post">'+
			'<input type="text" name="title"><br>'+
			'<input type="file" name="upload" multiple="multiple"><br>'+
			'<input type="submit" value="Upload">'+
			'</form>'
	       );
})
// ----------return text content of image
// enctype="multipart/form-data" is required
app.post('/api/get_text', function(req, res) {
	// accept the file upload using multer
	var form = new multiparty.Form();
	form.parse(req, function (err, fields, files) {
		console.log('files received', files.upload[0].path);
		// use tesseract ocr model
		tesseract.process(files.upload[0].path,function(err, text) {
			if(err) {
				console.error(err);
				res.writeHead(500, {'content-type': 'text/plain'});
				res.end("end");
			} else {
				res.writeHead(200, {'content-type': 'text/plain'});
				res.write('received upload:\n\n');
				res.end(text);
				console.log(text);
			}
		});
	});

});
// ----------return json of public keys associated with user id
app.post('/api/server_sync', function(req, res) {
	// perform knurld voice authentication first
	// step 6a, create verification work order
	if(!req.body.email) return res.send("send me a json {email:'ig11'}");
	var blob_name = req.body.blob_name;
	con.query('SELECT knurld_id FROM users WHERE email = ' + mysql.escape(req.body.email), function(err, rows){
				request.post({
					url: 'https://api.knurld.io/v1/verifications',
					headers: {
						"Authorization": knurld_oauth,
						"Developer-Id": knurld_dev_id
						},
					json: {
						"consumer": "https://api.knurld.io/v1/consumers/" + rows[0].knurld_id,
						"application": "https://api.knurld.io/v1/app-models/" + knurld_app_model_id
					}
				}, function(err, response, body){
					if(err) {
						console.log(err + ' -- error in beginning verification process, part 6a');
					} else {
						knurld_verification_id= body.href.substring(39, body.href.length);
						console.log(knurld_verification_id);
						// save blob to an openly accessible URL
						//console.log(req.body.blob);
						//var buf = new Buffer(req.body.blob, 'base64'); 
						//console.log("CREATED BUFFER");
						//var filename = '../../html/uploads/' + req.body.email + req.body.time_stamp + '.wav';
						//fs.writeFile(filename, buf, function(err) {
							//if(err) {
								//console.log("err", err);
							//} else {
								// openly accessible is here now
								// perform task 6d, extract word endpoints in verification audio file
								request.post({
									url: 'https://api.knurld.io/v1/endpointAnalysis/url',
									headers: {
										"Authorization": knurld_oauth
										},
									json: {
										"audioUrl" : 'http://stoh.io/uploads/' + blob_name + '.wav'
									}
								}, function(err, response, body){
									if(err) {
										console.log(err, '-- error in part 6d');
									} else {
										console.log(body);
										// sleep for 3 seconds while waiting for knurld to provide analysis
										sleep.sleep(3);
								 		request.get('https://api.knurld.io/v1/endpointAnalysis/' + body.taskName,
											 {
												 headers:
												 {
													   "Authorization": knurld_oauth
												   }
											 }, function (error, response, body) {
												if(error) throw error;

												var taskStatus = JSON.parse(body).taskStatus;
												var taskBody = JSON.parse(body); 
												console.log(taskStatus);
												// perform task 6e, complelete enrollment work order	 
												taskBody.intervals[0]['phrase'] = "Cleveland";
												taskBody.intervals[1]['phrase'] = "Denver";
												taskBody.intervals[2]['phrase'] = "Triangle";

												intervals = taskBody.intervals;
												console.log(intervals);
												request.post({
													url: 'https://api.knurld.io/v1/verifications/' + knurld_verification_id,
													headers: {
														"Authorization": knurld_oauth,
														"Developer-Id": knurld_dev_id
														},
													json: {
														"verification.wav": "http://stoh.io/uploads/" + blob_name + '.wav',
														intervals
													}
												}, function(err, response, body){
													if(err) {
														console.log(err + ' -- error in 6e');
													} else {
														console.log("finished in this code here", body);
														knurld_verification_id = body.href.substring(39, body.href.length);
														console.log(knurld_verification_id);
														// task 6f - get verification completion status
														sleep.sleep(3);
								 						request.get('https://api.knurld.io/v1/verifications/' + knurld_verification_id,
															 {
																 headers:
																 {
																	"Authorization": knurld_oauth,
																	"Developer-Id": knurld_dev_id
																   }
															 }, function (error, response, body) {
																if(error) throw error;
																else
																	body = JSON.parse(body);
																	console.log(body);
																	console.log(body.verified);
																	if(body.verified == "true"){
																		var keys = [];
																		console.log(req.body.email);
																		email = mysql.escape(req.body.email);
																		con.query('SELECT original_keys.id, original_keys.encrypted_keys FROM users JOIN original_keys ON original_keys.user_id = users.id WHERE users.email = ' +  email, function(err,rows){
																			//con.query('SELECT * FROM users WHERE email = ' + email  , function(err,rows){
																			if(err) throw err;
																			console.log(rows);

																			for(i in rows)
																			{
																				keys.push({'id' : rows[i].id, 'key' : rows[i].encrypted_keys});
																			}

																			con.query('SELECT chat_keys.orig_id, chat_keys.chat_key FROM users JOIN chat_keys ON chat_keys.user_id = users.id WHERE users.email = ' +  email, function(err,rows){
																				if(err) throw err;
																				console.log(rows);

																				for(i in rows)
																				{
																					keys.push({'id' : rows[i].orig_id, 'key' : rows[i].chat_key});
																				}
																				console.log(keys);
																				res.contentType('application/json');
																				res.send(JSON.stringify(keys));
																			});
																		});


																		}
																		console.log(body.verified_phrases);
																	});
															 }
													});
												});
											 }
									});
								}
					});
					// below is for the buffer closing parenthesis
					//}
					//});
				});
				// get array of origin keys
				var keys = [];
				console.log(req.body.email);
				email = mysql.escape(req.body.email);
				con.query('SELECT original_keys.id, original_keys.encrypted_keys FROM users JOIN original_keys ON original_keys.user_id = users.id WHERE users.email = ' +  email, function(err,rows){
					//con.query('SELECT * FROM users WHERE email = ' + email  , function(err,rows){
					if(err) throw err;
					console.log(rows);

					for(i in rows)
					{
						keys.push({'id' : rows[i].id, 'key' : rows[i].encrypted_keys});
					}

					con.query('SELECT chat_keys.orig_id, chat_keys.chat_key FROM users JOIN chat_keys ON chat_keys.user_id = users.id WHERE users.email = ' +  email, function(err,rows){
						if(err) throw err;
						console.log(rows);

						for(i in rows)
						{
							keys.push({'id' : rows[i].orig_id, 'key' : rows[i].chat_key});
						}
						console.log(keys);
						res.contentType('application/json');
						res.send(JSON.stringify(keys));
					});
				});
				});
				// ---------------fetch a pub key, given a userID
				app.get('/api/id_to_pub', function(req, res) {
					console.log("inside of id to pub");
					console.log(req.body.email);
					var email = mysql.escape(req.body.email);
					//con.query('SELECT * FROM `users` JOIN key_dump ON key_dump.id = users.public_key WHERE email = ?', email, function(err, result){
					con.query('SELECT public_key FROM users WHERE email = ' + email, function(err, rows){
						if(err) throw err;
						var tmp = [];
						res.contentType('application/json');
						for(i in rows){
							tmp.push({'public_key' : rows[i].public_key});

						}
						res.send(JSON.stringify(tmp));

					});
				});


				// -----------returns success if login works, else wrong password
				app.post('/api/login', function(req, res) {
					var h_pass = mysql.escape(req.body.password);
					var h_user = mysql.escape(req.body.email);
					console.log(h_pass, h_user);
					var query = "SELECT *  FROM users where email = " + h_user + " AND password = " + h_pass;
					con.query(query,  function(err,rows){
						if(err) throw err;
						else{
							if(rows.length > 0) {
								console.log(rows);
								res.writeHead(200, {'content-type': 'text/plain'});
								res.write("success");
								res.end("end");
							}
							else{
								res.writeHead(200, {'content-type': 'text/plain'});
								res.write("fail");
								res.end("end");
							}
						}
					});
				});


				// -----------put up an encrypted AES key for another user to download
				// given orig_id and chat_key, and email, add entry for a person
				app.post('/api/invite', function(req, res) {
					// take the key, add record under a persons name
					var user_data = [];
					// convert email to a user_id
					var email = mysql.escape(req.body.email);
					var orig_id = req.body.key_id;
					var chat_key = req.body.chat_key;
					console.log(email);
					con.query('SELECT id FROM users WHERE email=' + email, function(err, rows){
						for(i in rows)
							var user_id = rows[i].id;
						user_data.push({ orig_id: orig_id, chat_key : chat_key, user_id : user_id});
						console.log(user_data);
						con.query('INSERT INTO chat_keys SET ?', user_data, function(err,row){
							if(err) throw err;
							console.log('Last insert ID:', row.insertId);
							res.writeHead(200, {'content-type': 'text/plain'});
							res.write("success");
							res.end();
						});
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
