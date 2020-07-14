var express = require('express');
var router = express.Router();

const { Board, Led } = require("johnny-five");
const board = new Board();
board.on("ready", () => {
    console.log('READY!');
    // var led = new Led(13);
    // if(status == 'on') led.on();
    // else led.off();
});

/* GET home page. */
router.get('/', function(req, res, next) {
    console.log('Current Status is : ' + req.query.status);
    const led = new Led(13);
    var msg = '';
    switch (req.query.status) {
        case 'on':
            led.on();
            msg = 'You turned it on!'
            break;
        case 'off':
            led.off();
            msg = 'You made it off';
            break;
        case 'blink':
            msg = 'You made it blinking *_*';
            led.blink();
            break;
        case 'stop':
            msg = 'Cycle complete, you stopped it!';
            led.stop();
            break;
    }
    res.send(msg);
});

module.exports = router;

































// 1	
// app : req.app : req.app
// req.baseUrl : req.baseUrl
// req.body : 
// req.cookies
// req.fresh
// req.hostname
// req.ip
// req.ips
// req.originalUrl
// 10	
// req.params

// An object containing properties mapped to the named route “parameters”. For example, if you have the route /user/:name, then the "name" property is available as req.params.name. This object defaults to {}.

// 11	
// req.path

// Contains the path part of the request URL.

// 12	
// req.protocol

// The request protocol string, "http" or "https" when requested with TLS.

// 13	
// req.query

// An object containing a property for each query string parameter in the route.

// 14	
// req.route

// The currently-matched route, a string.

// 15	
// req.secure

// A Boolean that is true if a TLS connection is established.

// 16	
// req.signedCookies

// When using cookie-parser middleware, this property contains signed cookies sent by the request, unsigned and ready for use.

// 17	
// req.stale

// Indicates whether the request is "stale," and is the opposite of req.fresh.

// 18	
// req.subdomains

// An array of subdomains in the domain name of the request.

// 19	
// req.xhr

// A Boolean value that is true if the request’s "X-Requested-With" header field is “XMLHttpRequest”, indicating that the request was issued by a client library such as jQuery.