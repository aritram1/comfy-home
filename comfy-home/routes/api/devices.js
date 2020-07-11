var express = require('express');
var router = express.Router();
const fs = require('fs');
var path = require('path');
var filePath = path.join(__dirname, '../../data/joystick.csv')

// GET devices listing


// Get Joystick data
router.get('/joystick', function(req, res, next) {
    fs.readFile(filePath, 'utf-8', function(err, data){ //first err, then data
        if(!err){
            responseData = [];
            count = 0;
            lines = data.split('\n');
            num = parseInt(req.query.num) > 0 ? parseInt(req.query.num) : lines.length;
            console.log(`First ${num} records sent`);
            lines.forEach(line =>{
                if(count < num){
                    if(line != ''){
                        time = line.split(' ')[0]
                        dt = line.split(' ')[1];
                        value = dt.substring(0, dt.length-1)
                        responseData.push({
                            time : time, 
                            data: {
                                X : parseInt(value.split(',')[0])-500,
                                Y : parseInt(value.split(',')[1])-500,
                                Switch : parseInt(value.split(',')[2])
                            }
                        })
                    }
                }
                count = count + 1;
            })
            res.send(responseData);
        }
        else{
            console.log(err, 'Please try after some time!!');
        }
         
    });
});

module.exports = router;