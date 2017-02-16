var io = require('socket.io').listen(80);

io.sockets.on('connection', function (socket) {
  socket.emit('news',  './tech 1 0' );
  socket.on('my other event', function (data) {
    console.log(data);
  });
});

