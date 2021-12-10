// YOU CAN USE THIS FILE AS REFERENCE FOR SERVER DEVELOPMENT

// include the express modules
var express = require("express");

// create an express application
var app = express();
const url = require('url');

// helps in extracting the body portion of an incoming request stream
var bodyparser = require('body-parser');

// fs module - provides an API for interacting with the file system
var fs = require("fs");

// helps in managing user sessions
var session = require('express-session');

// include the mysql module
var mysql = require("mysql");

// Bcrypt library for comparing password hashes
const bcrypt = require('bcrypt');

// A possible library helps reading uploaded file.
var formidable = require('formidable')

// for reading XML files
var xml2js = require('xml2js');


// apply the body-parser middleware to all incoming requests
app.use(bodyparser());

// use express-session
// in mremory session is sufficient for this assignment
app.use(session({
  secret: "csci4131secretkey",
  saveUninitialized: true,
  resave: false
}
));

// get the DataBase info from XML file
var parser = new xml2js.Parser();
var xml;

const content = fs.readFileSync('dbconfig.xml','utf8');
parser.parseString(content, function (err, result) {
	if(err) throw err;
		xml = result;
	});

var host = xml.dbconfig.host[0];
var user = xml.dbconfig.user[0]; 
var password = xml.dbconfig.password[0];
var database = xml.dbconfig.database[0];
var port = xml.dbconfig.port[0];

console.log("info: " + host + " " + user + " " + password + " " + database + " " + port);

// Connect to DataBase
var connection = mysql.createConnection({

		 host: host,
		 user: user,       
		 password: password,   
		 database: database,   
		 port: port
});

connection.connect(function(err) {
	if(err) {
		console.log("bad connection");
		throw err;
	};
	console.log("Connected to MYSQL database!");
});

// server listens on port 9007 for incoming connections
app.listen(9007, () => console.log('Listening on port 9007!'));


// function to return the welcome page
app.get('/',function(req, res) {
  res.sendFile(__dirname + '/client/welcome.html');
});

app.get('/admin',function(req, res) {

	// redirect to login if not logged in.
	if(!req.session.value){
		res.redirect('/login');
	}

	else {
		req.session.value += 1;
		var newval = req.session.value;
		res.sendFile(__dirname + '/client/admin.html');
	}
});

// function to return the login page
app.get('/login',function(req, res) {
	
	// validate user account info
	if(!req.session.value) {
		res.sendFile(__dirname + '/client/login.html');
	}

	else {
		req.session.value += 1;
		//res.sendFile(__dirname + '/client/schedule.html');
		res.redirect('/allEvents');

	}

});

//function to return the allEvents page
app.get('/allEvents',function(req, res) {
	
	// redirect to login if not logged in.
	if(!req.session.value){
		res.redirect('/login');
	}

	else {
		req.session.value += 1;
		var newval = req.session.value;
		res.sendFile(__dirname + '/client/allEvents.html');
	}
});

app.get('/schedule',function(req, res) {

	// redirect to login if not logged in.
	if(!req.session.value){
		res.redirect('/login');
	}

	else {
		req.session.value += 1;
		var newval = req.session.value;
		res.sendFile(__dirname + '/client/schedule.html');
	}
	


});

app.get('/logout',function(req, res) {
	// kill the session and send login
	req.session.destroy();
	res.redirect('/login');
});

app.get('/addEvent',function(req, res) {

	// redirect to login if not logged in.
	if(!req.session.value){
		res.redirect('/login');
	}

	else {
		req.session.value += 1;
		var newval = req.session.value;
		res.sendFile(__dirname + '/client/addEvent.html');
	} 
});

app.get('/getListOfUsers', function(req, res) {
	
	// redirect to login if not logged in	
	if(!req.session.value) {
		res.redirect('/login')
	}

	else {
		
		connection.query('SELECT * FROM tbl_accounts', function(err, rows, fields) {
			if(err) throw err;
			if(rows.length == 0)
				console.log("No entries in tbl_accounts");

			// build an array of json objects containing account info
			var accArray = [];
			for(var i = 0; i < rows.length; i++) {
				
				var obj = {

					id:rows[i].acc_id,
					name:rows[i].acc_name,
					login:rows[i].acc_login,
					password:rows[i].acc_password
				};
				accArray.push(obj);
			}
			// ship back to client
			res.json(accArray);

		});	
	}

});

app.get('/userLogin', function(req, res) {
	res.send({"login":req.session.user});
});



// middle ware to serve static files
app.use('/client', express.static(__dirname + '/client'));


// function to return the 404 message and error to client
app.get('*', function(req, res) {
  // add details
  res.sendStatus(404);
});



app.post('/addUser', function(req, res) {
	
	if(req.session.value) {
		
		connection.query('SELECT acc_login FROM tbl_accounts WHERE acc_login = ?',req.body.login, function(err, rows, fields) {
			if(err) throw err;
			// username does not exist
			if(rows.length === 0) {
				const insert = {
					acc_name: req.body.name,
					acc_login: req.body.login,
					acc_password: req.body.password
				};

				// insert into tbl_accounts
				connection.query('INSERT tbl_accounts SET ?', insert, function(err, result) {
					if(err) throw err;
					console.log("Table Record Inserted!");
					res.send({flag:true, id: result.insertId})
				});

			}

			else{
				// username exists
				res.send({flag:false});
			}
		}) 
	}

	else { 
		res.redirect('/login');
	}
});

app.post('/updateUser', function(req, res) {
	
	if(req.session.value) {
		console.log("before first query");
		connection.query('SELECT * FROM tbl_accounts WHERE acc_login = ? AND acc_id != ?', [req.body.login, req.body.id], function(err, rows, fields) {
			if(err) throw err;
			
			if(rows.length === 0) {
				

				if(req.body.password !== "") { // update everything

					const passwordHash = bcrypt.hashSync(req.body.password, 10);
					var query = 'UPDATE tbl_accounts SET acc_name = ?, acc_login = ?, acc_password = ? WHERE acc_id = ?';
					connection.query(query, [req.body.name, req.body.login, passwordHash, req.body.id], function(err, rows, fields) {
						if(err) throw err;
						console.log("all account info updated");
					});

				}

				else { // Don't update password
					var query = 'UPDATE tbl_accounts SET acc_name = ?, acc_login = ? WHERE acc_id = ?';
					connection.query(query, [req.body.name, req.body.login, req.body.id], function(err, rows, fields) {
						if(err) throw err;
						console.log("Name and Login updated");
					});
				}
				res.send({flag:true});
			}

			else {
				// id matches up
				res.send({flag:false});
			}
		});
	}

	else { // No user logged in
		res.redirect('/login');
	} 
});

app.post('/deleteUser', function(req, res) {
	console.log("in delete User post");

	if(req.session.value) {

		// not a currently logged in user
		if(req.body.login != req.session.user){

			connection.query('DELETE FROM tbl_accounts WHERE acc_login = ?',req.body.login, function(err, rows, fields) {
				if(err) throw err;
				console.log("delete successful");
				res.send({flag:true});
			});
		}
		else {
			res.send({flag:false});
		}


	}

	else {
		res.redirect('/login');
	}

});


// used for both schedule.html and allEvents.html
app.post('/getAllEvents',function(req, res) {

	// build JSON from database info
	connection.query('SELECT * FROM tbl_events',function(err,rows,fields) {
		if(err) throw err;
		if(rows.length == 0) {
			console.log("No entries in tbl_events");
		}
		// query the data base and send our info
		else {

			var jsonTemplate = {"monday": [],"tuesday": [], "wednesday": [], "thursday": [], "friday": [], "saturday": [], "sunday": []};

			// push every event in the data base to json to be sent to client
			for(var i = 0; i < rows.length; i++) {

				var day = rows[i].event_day;
				var name = rows[i].event_event;
				var start = rows[i].event_start;
				var end = rows[i].event_end;
				var location = rows[i].event_location;
				var phone = rows[i].event_phone;
				var info = rows[i].event_info;
				var url = rows[i].event_url;

				var json = {};
			    json.name = name;
			    json.start = start;
			    json.end = end;
			    json.phone = phone;
			    json.location = location;
			    json.info = info;
			    json.url = url;

			   
			    day = day.toLowerCase();
			    jsonTemplate[day].push(json);

			    // sort by time:
		        jsonTemplate[day] = jsonTemplate[day].sort(function(a,b){
		            return a.start.localeCompare(b.start);
		        });

			}

		    res.json(JSON.stringify(jsonTemplate));

		}
	});

});

// process POST
app.post('/login',function(req, res) {
	var loginInfo = req.body;
	var login = loginInfo.login;
	var pwd = loginInfo.password;

	
	// DATA BASE CODE
	connection.query('SELECT * FROM tbl_accounts',function(err,rows,fields) {
	if(err) throw err;
	if(rows.length == 0) {
		console.log("No entries in tbl_accounts");
		res.json({status: 'fail'});
	}

	// compare login info and return success/fail
	var pwdResult = bcrypt.compareSync(pwd,rows[0].acc_password)

	if(pwdResult && (login === rows[0].acc_login)){
		
		req.session.user = login;
		req.session.value = 1;
		res.json({status: 'success'});
		
	}
	else{
		res.json({status: 'fail'});
	}

	});
	// DATA BASE CODE END

	
});

app.post('/postEventEntry',function(req, res) {

	
	var postObj = req.body;


    // create variables for all the form data.
    var day = postObj.day;
    var name = postObj.event;
    var start = postObj.start;
    var end = postObj.end;
    var phone = postObj.phone;
    var location = postObj.location;
    var info = postObj.info;
    var url = postObj.url;

    // Insert all of our form data into tbl_events from database and redirect to schedule page
    const rowToBeInserted = {
        event_day: day, 
        event_event: name,       
        event_start: start,
        event_end: end,
        event_location: location,
        event_phone: phone,
        event_info: info,
        event_url: url  
    };

    console.log("Attempting to insert record into tbl_events");
    connection.query('INSERT tbl_events SET ?', rowToBeInserted, function (err, result) {
    	if (err) {
    		throw err;
    	}
    	console.log("Table record inserted");
    });
 
	res.redirect('/allEvents');
});


