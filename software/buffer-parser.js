module.exports = function (debug, splitOnOnlyNewline) {
  var data = new Buffer(0);

  return function (emitter, buffer) {
    var newlineIndexes = [];
    data = Buffer.concat([data, buffer]);

    if (debug) {
      console.log('Data is', data);
    }

    for(var i = 0; i < data.length; i++) {
      if ((splitOnOnlyNewline && data[i] == 0x0A) ||
        (!splitOnOnlyNewline && data[i] == 0x0D && data[i + 1] == 0x0A)) {
        newlineIndexes.push(i);
      }
    }

    if (newlineIndexes.length) {
      if (debug) {
        console.log('newlineIndexes', newlineIndexes)
      }

      for (var i = 0; i < newlineIndexes.length; i++) {
        var startSlice, endSlice;

        if (i === 0) { // first slice
          startSlice = 0;
          endSlice = newlineIndexes[i];
        } else {
          startSlice = newlineIndexes[i] + 1;
          endSlice = newlineIndexes[i + 1];
        }

        if (typeof endSlice === 'undefined') {
          break;
        }

        var slice = data.slice(startSlice, endSlice);

        if (debug) {
          console.log('Slicing from', startSlice, 'to', endSlice);
          console.log('Slice:', slice);
        }

        emitter.emit('data', slice);
      }

      data = data.slice(newlineIndexes[newlineIndexes.length - 1] + 2);

      if (debug) {
        console.log('data is now', data);
      }
    }
  };
}