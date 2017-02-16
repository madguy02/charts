var spawn = require('child_process').spawn;
var cmd  = spawn('python', ['gui.py']);
var counter = 0;
cmd.stdout.on('data', function(data) {
    counter ++;
  console.log('stdout: ' + data);
});

cmd.stderr.on('data', function(data) {
  console.log('stderr: ' + data);
});

cmd.on('exit', function(code) {
  console.log('exit code: ' + code);
  console.log(counter);
});