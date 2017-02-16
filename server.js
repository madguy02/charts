
var http = require('http');
var fs = require('fs');
var path = require('path');
var ws = fs.createWriteStream("temp.txt");



 var server = http.createServer(function (request, response) {
require('events').EventEmitter.prototype._maxListeners = 10;
   console.log('request starting...');

    var filePath = '.' + request.url;
    if (filePath == './')
        filePath = './line.html';
    var extname = path.extname(filePath);
    var contentType = 'text/html';
    switch (extname) {
        case '.js':
            contentType = 'text/javascript';
            break;
        case '.css':
            contentType = 'text/css';
            break;
        case '.json':
            contentType = 'application/json';
            break;
        case '.png':
            contentType = 'image/png';
            break;      
        case '.jpg':
            contentType = 'image/jpg';
            break;
        case '.wav':
            contentType = 'audio/wav';
            break;
    }

/*var du = io.on('connection', function(socket){
console.log('A user connected'); */

var listener = io.listen(server);
listener.sockets.on('connection',function(socket){
 var data = [{
                        "sale": "200",
                        "year": "2000"
                    }, {
                        "sale": "120",
                        "year": "2002"
                    }, {
                        "sale": "190",
                        "year": "2004"
                    }, {
                        "sale": "170",
                        "year": "2006"
                    }, {
                        "sale": "210",
                        "year": "2008"
                    }, {
                        "sale": "140",
                        "year": "2010"
                    }];  
                   
socket.json.send('message',data);
//socket.close();
});


  //Whenever someone disconnects this piece of code executed
  //socket.on('disconnect', function () {
    //console.log('A user disconnected');
  //}); 

//});
    fs.readFile(filePath, function(error, content) {
        if (error) {
            if(error.code == 'ENOENT'){
                fs.readFile('./404.html', function(error, content) {
                    response.writeHead(200, { 'Content-Type': contentType });
                    response.end(content, 'utf-8');
                });
            }
            else {
                response.writeHead(500);
                response.end('Sorry, check with the site admin for error: '+error.code+' ..\n');
                response.end(); 
            }
        }
        else {
            response.writeHead(200, { 'Content-Type': contentType });
            response.end(content, 'utf-8');
        }
    });

})//.listen(8125);
var io = require('socket.io')(server);
server.listen(8125);
console.log('Server running at http://127.0.0.1:8125/');
