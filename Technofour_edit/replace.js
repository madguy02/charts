var replace = require("replace");

replace({
 
   	regex: "10",
    replacement: "30",
    regex: "3000000",
    replacement: "1000000",
    regex: "6300.0",
    replacement: "6320.0",
    regex: "5000000",
    replacement: "1000000",
    regex: "4",
    replacement:"1",
   
	paths: ['build/technofour_config.cpp'],
    recursive: true,
    silent: true,
});