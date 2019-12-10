//Express framework
var express = require('express');
var app = express();
//MySql Setup
var mysql = require('mysql');
var pool      =    mysql.createPool({
    connectionLimit : 100, //important
    host     : 'localhost',
    user     : 'root',
    password : '123456',
    database : 'db',
    port     : 3306,
    debug    :  false
});



function handle_database(req,res) {
    var q = url.parse(req.url, true).query;
    var txt = q.machine + " " + q.machineKey;
    
    var sql = 'insert into machine_counter (machine_name_id,time_trigged) select id,now(6) from machine where machine_name = ? and machine_key = ? limit 1;';
    var sqlParams = [q.machine, q.machineKey];
    
    
       // connection will be acquired automatically
       pool.query(sql,sqlParams,function(err,rows){
        if(err) {
            return res.json({'error': true, 'message': 'Error occurred'+err});
        }
           response = {
              MachineName:q.machine,
              MachineKey:req.query.machineKey,
              UpdateSuccessful:rows.affectedRows
           };
           res.json(response);
                //connection will be released as well.
                //res.json(rows); 
       });
}

app.get("/",function(req,res){-
        handle_database(req,res);
});

var server = app.listen(3001, function () {
   var host = server.address().address
   var port = server.address().port
   
   console.log("Machine Counter App listening at http://%s:%s", host, port)
})




//old server

var http = require('http');
var url = require('url');
    
http.createServer(function (req, res) {
    
    var q = url.parse(req.url, true).query;
    var txt = q.machine + " " + q.machineKey;

    var connection = mysql.createConnection({
      host     : 'localhost',
      user     : 'root',
      password : '',
      database : 'db',
      port     : 3306
    });

    connection.connect();
     
    var sql = 'insert into machine_counter (machine_name_id) select id from machine where machine_name = ? and machine_key = ?;';
    var sqlParams = [q.machine, q.machineKey];
    
    connection.query(sql,sqlParams,function (err, result) {
            if(err){
              console.log('[SELECT ERROR] - ',err.message);
              return;
            }
     
            console.log('--------------------------INSERT----------------------------');
            //console.log('INSERT ID:',result.insertId);        
            console.log('INSERT ID:',result);   
            txt=txt+" "+result.toString();
            console.log('-----------------------------------------------------------------\n\n');  
    });
     
    connection.end();
    //connection.destroy();
    txt=txt +" hihi";
    res.writeHead(200, {'Content-Type': 'text/plain'});
    res.end(txt);

    
}).listen(8080);