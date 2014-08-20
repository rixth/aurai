var EventEmitter = require('events').EventEmitter,
    SerialPort = require('serialport'),
    bufferParser = require('./buffer-parser'),
    util = require('util');

function AC(serialPort) {
  this.running = false;
  this.temp = null;
  this.mode = null;
  this.fanSpeed = null;
  this.environment = {};
  this.resetTimer;
  this.serialPortPath = serialPort;
  // this.firePortCycleTimer();
  this.queue = [];

  this.serialPort = new SerialPort.SerialPort(this.serialPortPath, {
    baudrate: 9600,
    parser: bufferParser(true, false)
  });

  this.serialPort.on('data', function (data) {
    console.log('Got', data);

    // data[0] = CMD_OK/FAIL
    // data[1] = SEND_OK/FAIL
    // data[2] = RESP_OK/TO
    // data[3] = SIZE
    // data[4..4+SIZE] = DATA

    if (data[0] != 0x03) {
      this.emit('lastCommandFailed');
      console.error('Command rejected by hub');
      return;
    }

    if (data[1] != 0x01) {
      this.emit('lastCommandFailed');
      console.error('Transmission from hub to spoke failed');
      return;
    }

    if (data[2] != 0x04) {
      this.emit('lastCommandFailed');
      console.error('Spoke did not respond after command was sent');
      return;
    }

    console.log('Got', data[3], 'bytes back from spoke');

    var payload = data.slice(4);

    if (payload[0] == 0x1 || payload[0] == 0x4) {
      this.emit('lastCommandOk');
      this._parseState(payload.slice(1));
    } else if (payload[0] == 0x2) {
      this.emit('lastCommandFailed');
      console.error('Spoke rejected the command');
      return;
    } else if (payload[0] == 0x8) {
      this.emit('lastCommandOk');
      this._parseEnv(payload.slice(1));
      this.emit('statusUpdated');
    } else {
      console.error('Unknown payload', payload);
    }
  }.bind(this));


  this.on('debugData', function (data) {
    console.log('DEBUG:', data.slice(1).toString());
  }.bind(this));

  this.on('lastCommandFailed', function () {
    if (this.lastCallback) {
      console.log(this.lastCallback)
      this.lastCallback(false);
      this.lastCallback = null;
    }
  }.bind(this));

  this.on('lastCommandOk', function () {
    if (this.lastCallback) {
      this.once('statusUpdated', function () {
        this.lastCallback(true);
        this.lastCallback = null;
      }.bind(this));
    }
  }.bind(this));
}


AC.prototype = Object.create(EventEmitter.prototype);

AC.prototype.firePortCycleTimer = function () {
  var hadPort = !!this.serialPort;
  if (this.serialPort) {
    this.serialPort.close();
  }
  this.serialPort = new SerialPort.SerialPort(this.serialPortPath, {
    baudrate: 9600,
    parser: bufferParser(false)
  });
  if (hadPort) {
    this.serialPort.open();
  }
  setTimeout(this.firePortCycleTimer.bind(this), 60000);
};

AC.prototype.status = function () {
  return {
    running: this.running,
    mode: this.mode,
    fanSpeed: this.fanSpeed,
    temp: this.temp,
    environment: this.environment
  };
};

AC.prototype.power = function (mode, cb) {
  this._sendCmd(commands.power[mode], cb);
};

AC.prototype.reset = function (cb) {
  this._sendCmd(commands.reset, cb);
};

AC.prototype.environmentFromChip = function (cb) {
  this._sendCmd(commands.environment, cb);
};

AC.prototype.statusFromChip = function (cb) {
  this._sendCmd(commands.status, cb);
};

AC.prototype.tempUp = function (cb) {
  this._sendCmd(commands.temperature.up, cb);
};

AC.prototype.tempDown = function (cb) {
  this._sendCmd(commands.temperature.down, cb);
};

AC.prototype.tempExact = function (temp, cb) {
  var buffer = new Buffer(4);
  buffer.writeUInt8(0x01, 0);
  buffer.writeUInt8(commands.temperature.exact, 1);
  buffer.writeUInt8(temp, 2);
  buffer.writeUInt8(0x0A, 3);
  this._sendCmd(buffer, cb);
};

AC.prototype.setFanSpeed = function (speed, cb) {
  this._sendCmd(commands.fanSpeed[speed], cb);
};

AC.prototype.setPowerTimer = function (onOrOff, hours, cb) {
  var buffer = new Buffer(2);
  buffer.writeUInt8(commands.timer[onOrOff], 0);
  buffer.writeUInt8(hours, 1);
  this._sendCmd(buffer, cb);
};

AC.prototype.setMode = function (mode, cb) {
  this._sendCmd(commands.mode[mode], cb);
};

AC.prototype._sendCmd = function (bits, cb) {
  if (this.lastCallback) {
    return this.queue.push(arguments);
  }

  var buf;

  if (Buffer.isBuffer(bits)) {
    buf = bits;
  } else if (typeof bits === 'number') {
    buf = new Buffer(3);
    buf.writeUInt8(0x01, 0);
    buf.writeUInt8(bits, 1);
    buf.writeUInt8(0x0A, 2);
  } else if (Array.isArray(bits)) {
    buf = new Buffer(bits.length);
    bits.forEach(function (bit, idx) {
      buf.writeUInt8(bit, idx);
    });
  } else {
    throw new Error("don't know what to do with " + bits);
  }

  this.lastCallback = function () {
    cb.apply(this, arguments);
    if (this.queue.length) {
      args = this.queue.shift();
      console.log(args);
      this._sendCmd.apply(this, args);
    }
  }.bind(this);

  console.log('Writing', buf, 'to serial port');

  this.serialPort.write(buf, function (_err) {
    if (_err && this.lastCallback) {
      this.lastCallback(_err);
      this.lastCallback = null;
    }
  }.bind(this));
}

AC.prototype._parseState = function (buffer) {
  var statusInt = buffer.readUInt16BE(0);
  this.temp = statusInt & 0xFF;
  this.fanSpeed = FAN_SPEEDS[((3 << 8) & statusInt) >> 8];
  this.mode = MODES[((3 << 10) & statusInt) >> 10];
  this.running = !!((1 << 12) & statusInt);
  this.emit('statusUpdated');
}

AC.prototype._parseEnv = function (buffer) {
  this.environment = {
    humidity: buffer.readInt8(0),
    temp: buffer.readInt8(1),
    updatedAt: Date.now(),
  };
  this.emit('environmentUpdated');
};

AC.prototype.init = function (cb) {
  this.serialPort.open(cb);
};

var commands = {
  power: {
    toggle: 0x08,
    on: 0x08 | 1,
    off: 0x08 | 2
  },
  mode: {
    next: 0x10,
    cool: 0x10 | 1,
    energySaver: 0x10 | 2,
    dry: 0x10 | 3,
    fan: 0x10 | 4
  },
  fanSpeed: {
    next: 0x18,
    low: 0x18 | 1,
    medium: 0x18 | 2,
    high: 0x18 | 3
  },
  temperature: {
    up: 0x20,
    down: 0x21,
    exact: 0x28
  },
  status: 0x30,
  reset: 0x50,
  environment: 0x48
};



var MODES = [
  'COOL',
  'ENERGY_SAVER',
  'FAN',
  'DRY'
];

var FAN_SPEEDS = [
  'LOW',
  'MED',
  'HIGH'
]

module.exports = AC;
