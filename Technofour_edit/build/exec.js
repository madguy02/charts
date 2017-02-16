var exec = require('child_process').exec, child;
var testscript = exec('nodejs server.js');
var execscript = exec('sudo ./tech 3.5 1');

/*testscript.stdout.on('data', function(data){
    console.log(data); 
    sendBackInfo();
});

testscript.stderr.on('data', function(data){
    console.log(data);
    triggerErrorStuff(); 
}); */
