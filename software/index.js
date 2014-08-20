var express = require('express');

var app = express();

app.use(express.static(__dirname + '/public'));

var AC = require('./ac');
var ac = new AC('/dev/' + (process.env.SERIAL_PORT || 'tty.SLAB_USBtoUART'));

app.get('/cmd/power/toggle', sendCmd.bind(this, ['power', 'toggle']));
app.get('/cmd/power/on', sendCmd.bind(this, ['power', 'on']));
app.get('/cmd/power/off', sendCmd.bind(this, ['power', 'off']));

app.get('/cmd/temp/up', sendCmd.bind(this, 'tempUp'));
app.get('/cmd/temp/down', sendCmd.bind(this, 'tempDown'));
app.get('/cmd/temp/exact/:target', function (req, res) {
  ac.tempExact(parseInt(req.params.target, 10), function (success) {
    res.send(JSON.stringify({
      success: success,
      status: ac.status()
    }));
  });
});

app.get('/cmd/fan-speed/next', sendCmd.bind(this, ['setFanSpeed', 'next']));
app.get('/cmd/fan-speed/low', sendCmd.bind(this, ['setFanSpeed', 'low']));
app.get('/cmd/fan-speed/medium', sendCmd.bind(this, ['setFanSpeed', 'medium']));
app.get('/cmd/fan-speed/high', sendCmd.bind(this, ['setFanSpeed', 'high']));

app.get('/cmd/mode/next', sendCmd.bind(this, ['setMode', 'next']));
app.get('/cmd/mode/cool', sendCmd.bind(this, ['setMode', 'cool']));
app.get('/cmd/mode/energySaver', sendCmd.bind(this, ['setMode', 'energySaver']));
app.get('/cmd/mode/dry', sendCmd.bind(this, ['setMode', 'dry']));
app.get('/cmd/mode/fan', sendCmd.bind(this, ['setMode', 'fan']));

app.get('/cmd/reset', sendCmd.bind(this, 'reset'));
app.get('/cmd/status', sendCmd.bind(this, 'statusFromChip'));

app.get('/cmd/environment', function (req, res) {
  ac.environmentFromChip(function (success, envData) {
    res.send(JSON.stringify({
      success: success,
      environment: envData
    }));
  });
});

app.get('/cmd/environment/log', function (req, res) {
  ac.environmentLog(20, function (success, logData) {
    res.send(JSON.stringify({
      success: success,
      log: logData
    }));
  });
});

ac.init(function () {
  app.listen(3000);
});


function sendCmd(cmd, req, res) {
  var args = [];

  if (Array.isArray(cmd)) {
    args = cmd.slice(1);
    cmd = cmd[0];
  }

  args.push(function (success) {
    res.send(JSON.stringify({
      success: success,
      status: ac.status()
    }));
  });

  ac[cmd].apply(ac, args);
}

var port = process.env.PORT || 3000;
app.listen(port);
console.log('Listening on port', port);
