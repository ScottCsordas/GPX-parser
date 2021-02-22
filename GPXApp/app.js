'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');
const mysql = require('mysql2/promise');
let connection;

app.use(fileUpload());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;

  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ********************\\

let sharedLib = ffi.Library('./libgpxparse',{
  validateGPXDoc:['int',['string', 'string']],
  writefromJSON:['void',['string', 'string']],
  routeToAttJSONlist:['string',['string', 'string']],
  fileNametoTrackListJSON:['string', ['string']],
  fileNametoRouteListJSON:['string', ['string']],
  fileNametoGPXJSON:['string',['string']],
});

app.get('/DBconnect', async function(req, res, next){
  try{
    connection = await mysql.createConnection({
      host     : 'dursley.socs.uoguelph.ca',
      user     : req.query.Uname,
      password : req.query.Pword,
      database : req.query.Uname
    })
    res.send(connection);
  }catch(e){
    console.log("Query error: "+e);
    res.send("could not connect try again");
  }
});

app.get('/display', async function(req, res, next){
  let retObj = new Object();
  retObj.numFiles = 0;
  retObj.numRoutes = 0;
  retObj.numPoints = 0;
  try{
    const [rows, fields, query] = await connection.query('SELECT COUNT(gpx_id) AS NUM_FILES FROM FILE')
      retObj.numFiles = rows[0].NUM_FILES;

    const [rows1, fields1, query1] = await connection.query('SELECT COUNT(route_id) AS NUM_ROUTES FROM ROUTE')
      retObj.numEvents = rows1[0].NUM_EVENTS;

    const [rows2, fields2, query2] = await connection.query('SELECT COUNT(point_id) AS NUM_POINTS FROM POINT')
      retObj.numAlarms = rows2[0].NUM_ALARMS;

    res.send(retObj);
  }catch(e){
    console.log("Query error: " +e);
    res.send({});
  }

});

app.get('/tableAdd', function(req, res) {

  try{
      const [rows1, fields1, query1] = connection.query("INSERT INTO FILE (file_name, ver, creator) VALUES ('"+req.query.file+"', "+req.query.version+", '"+req.query.creator+"')");

      //const [rows2, fields2, query2] = connection.query("SELECT gpx_id FROM FILE WHERE(file_name = '"+req.query.file+"')");
      res.send(connection.query("SELECT gpx_id FROM FILE WHERE(file_name = '"+req.query.file+"')"));

    }catch(e){
      console.log("Query error: " +e);
      res.send({});
    }
});

app.get('/addRoute', function(req, res) {

  try{

      const [rows1, fields1, query1] = connection.query("INSERT INTO ROUTE (route_name, route_len, gpx_id) VALUES ('"+req.query.route+"', "+req.query.len+", "+req.query.gpx_id+")");

      const [rows2, fields2, query2] = connection.query("SELECT route_id FROM ROUTE WHERE(route_name = '"+req.query.route+"')");
      res.send({route_id: result[0].route_id})


    }catch(e){
      console.log("Query error: " +e);
      res.send({});
    }
});


app.get('/clear', function(req, res) {
  try{
    const [rows, fields, query] = connection.query("DELETE FROM FILE");
    res.send({});
  }catch(e){
    console.log("Query error: " +e);
    res.send({});
  }
});

app.get('/makeTables', async function(req, res, next){
  try{
    const [rows, fields, query] = await connection.query("CREATE TABLE FILE( \
        gpx_id INT AUTO_INCREMENT PRIMARY KEY,\
        file_Name VARCHAR(60) NOT NULL,\
        ver DECIMAL(2,1) NOT NULL,\
        creator VARCHAR(256) NOT NULL)");

        res.send({});
  }
  catch(e){
    console.log("Query error: " +e);
    res.send({});
  }
});

app.get('/makeRouteTable', async function(req, res, next){
  try{
    const [rows2, fields2, query2] = await connection.query("CREATE TABLE ROUTE( \
        route_id INT AUTO_INCREMENT PRIMARY KEY,\
        route_Name VARCHAR(256) NOT NULL,\
        route_len FLOAT(15,7) NOT NULL,\
        gpx_id INT NOT NULL,\
        FOREIGN KEY(gpx_id) REFERENCES FILE(gpx_id) ON DELETE CASCADE)");

        res.send({});
  }
  catch(e){
    console.log("Query error: " +e);
    res.send({});
  }
});

app.get('/makePointTable', async function(req, res, next){
  try{
    const [rows2, fields2, query2] = await connection.query("CREATE TABLE POINT( \
        point_id INT AUTO_INCREMENT PRIMARY KEY,\
        point_index INT NOT NULL,\
        latitude DECIMAL(11,7) NOT NULL,\
        longitude DECIMAL(11,7)  NOT NULL,\
        point_name VARCHAR(256),\
        route_id INT NOT NULL,\
        FOREIGN KEY(route_id) REFERENCES ROUTE(route_id) ON DELETE CASCADE)");

        res.send({});
  }
  catch(e){
    console.log("Query error: " +e);
    res.send({});
  }
});

app.get('/routeAttJSON', function(req , res){
  let retStr = sharedLib.routeToAttJSONlist(req.query.compName, req.query.fileName);
  res.send(retStr);
});

app.get('/writeGPX', function(req , res){
  sharedLib.writefromJSON(req.query.json, req.query.fileName);
  let retStr = "";
  res.send(retStr);
});

app.get('/getJSON', function(req , res){
  let retStr = sharedLib.fileNametoGPXJSON(req.query.fileName);
  res.send(retStr);
});

app.get('/getRoutesJSON', function(req , res){
  let retStr = sharedLib.fileNametoRouteListJSON(req.query.fileName);
  res.send(retStr);
});

app.get('/getTracksJSON', function(req , res){
  let retStr = sharedLib.fileNametoTrackListJSON(req.query.fileName);
  res.send(retStr);
});

app.get('/getFiles', function(req , res) {
  const testFolder = 'uploads/';
  const fs = require('fs');

  fs.readdir(testFolder, (err, files) => {
    let fileList = [];

    for (let i = 0; i < files.length; i++) {
      let result = sharedLib.validateGPXDoc("uploads/"+files[i], "gpx.xsd");

      if (result == 1) {
        fileList.push(files[i]);
      }
    }
    res.send(fileList);
  });
});

//Sample endpoint
app.get('/someendpoint', function(req , res){
  let retStr = req.query.name1 + " " + req.query.name2;
  res.send({
    foo: retStr
  });
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
