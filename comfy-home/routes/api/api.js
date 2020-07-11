var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/devices', function(req, res, next) {
  let info = [
     {
       'name' : 'Device 1',
       'status' : 'Active'
     },
     {
      'name' : 'Device 2',
      'status' : 'Inactive'
     },
     {
      'name' : 'Device 3',
      'status' : 'Active'
     }
  ]
  res.send(info);
  //res.render('alldevices', {info: info});
  
});

router.get('/user', function(req, res, next) {
  info = {
    connection : req.get('connection'),
    userAgent: req.get('user-agent'),
    ip : req.ip,
    conn: req.header('connection'),
    headers: req.headers
  }
  console.log(info);
  res.send(info);
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