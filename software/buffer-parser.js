module.exports = function (debug) {
  var data = new Buffer(0),
      bufferIsEmpty = true,
      targetLength = null;

  // STATUS STATUS STATUS LENGTH [DATA...]
  var LENGTH_BYTE_IDX = 3,
      STATUS_SIZE = 3,
      PREAMBLE_SIZE = STATUS_SIZE + 1;

  return function self(emitter, buffer) {
    data = Buffer.concat([data, buffer]);

    if (debug && buffer.length > 0) {
      console.log('New data received. Data is:', data);
    }

    if (data.length < PREAMBLE_SIZE) {
      if (debug) {
        console.log('Less than', PREAMBLE_SIZE, 'bytes in data, returning');
      }
      return;
    }

    if (targetLength === null && data.length >= PREAMBLE_SIZE) {
      targetLength = data.readUInt8(LENGTH_BYTE_IDX) + PREAMBLE_SIZE;
      bufferIsEmpty = false;
      if (debug) {
        console.log('Target length is', targetLength);
      }
    }

    if (data.length >= targetLength) {
      var slice = data.slice(0, targetLength);
      data = data.slice(targetLength);
      if (debug) {
        console.log('Emitting:', slice);
      }
      emitter.emit('data', slice);
      targetLength = null;

      if (debug) {
        console.log('Data emitted. New data is:', data);
      }

      // edge case, say data now has 4 bytes: 0x02, 0xFA, 0xFB, 0x03, 0x0A, 0x0B
      // we need to make sure that the two-byte data is emitted too
      if (data.length >= PREAMBLE_SIZE && data.readUInt8(LENGTH_BYTE_IDX) <= (data.length + PREAMBLE_SIZE)) {
        if (debug) {
          console.log('Recursing');
        }
        self(emitter, new Buffer(0));
      }
    }
  };
}
