/*globals $, dewPoint */

'use strict';

var TEMP_MODE = localStorage.getItem('tempMode') || 'f';

function formatTemperature(degreeC) {
  if (!TEMP_MODE || TEMP_MODE === 'c') {
    return Math.round(degreeC)
  } else {
    return Math.round(degreeC * 1.8000 + 32.00);
  }
}

function renderStatus(status) {
  // $(document.body).toggleClass('no-fan-speed', status.mode === 'DRY').
  //   toggleClass('no-temp-target', status.mode === 'FAN').
  //   toggleClass('power-off', !status.running).
  //   toggleClass('power-on', status.running);

  $(document.body).toggleClass('power-off', !status.running);
  $(".mode .current").text(status.mode.toLowerCase().replace(/_/, ' ').replace(/energy saver/, 'eco'));
  $(".fan-speed .current").text(status.fanSpeed.toLowerCase().replace(/_/, ' '));
  $(".target-temperature .number").text(status.temp);
}

function renderEnvironment(environment) {
  $(".current-temperature .number").text(formatTemperature(environment.temperature));
  $(".humidity .number").text(environment.humidity);
  $(".dewpoint .number").text(formatTemperature(environment.dewPoint));
  $(".dewpoint .temperature-unit, .current-temperature .temperature-unit").text(TEMP_MODE.toUpperCase())
}

var setLoadingCount = (function () {
  var loadingCount = 0;

  return function (cnt) {
    loadingCount += cnt;
    $(document.body).toggleClass('loading', loadingCount > 0);
  };
})();

function runCmd(cmd, cb, hideLoader) {
  if (!hideLoader) {
    setLoadingCount(1);
  }
  $.getJSON('/cmd/' + cmd).then(function (d) {
    if (!hideLoader) {
      setLoadingCount(-1);
    }

    if (!d.success) {
      return alert('Connection successful but operation failed.');
    }

    if (d.status) {
      renderStatus(d.status);
    }

    if (d.environment) {
      d.environment.dewPoint = dewPoint(d.environment.humidity, d.environment.temperature);
      renderEnvironment(d.environment);
    }

    if (cb) {
      cb(d);
    }
  }, function () {
    if (!hideLoader) {
      setLoadingCount(-1);
    }
    alert('HTTP Connection error.');
  });
}

$('[data-cmd]').click(function (evt) {
  runCmd($(evt.currentTarget).attr('data-cmd'));
});

$('.temperature-unit').click(function () {
  toggleTempMode();
  localStorage.setItem('tempMode', TEMP_MODE);
})

function updateStatus() {
  runCmd('status', function () {
    setTimeout(runCmd.bind(this, 'environment'), 250);
  }, true);
}

function updateEnvironmentalTrends() {
  runCmd('environment/log', function (data) {
  var lookBack = 8,
      count = data.log.humidity.length,
      nowTemperature = data.log.temperature[count - 1],
      thenTemperature = data.log.temperature[count - lookBack];

    if (nowTemperature == thenTemperature) {
      $(document.body).removeClass('temperature-rising', 'temperature-falling');
    } else if (nowTemperature < thenTemperature) {
      $(document.body).removeClass('temperature-rising').addClass('temperature-falling');
    } else if (nowTemperature > thenTemperature) {
      $(document.body).removeClass('temperature-falling').addClass('temperature-rising');
    }
  }, true);
}

$('.reset a').click(function () {
  if(confirm("Reset AC state to: cool, 70F, fan speed low, off?")) {
    runCmd('reset');
  }
});

function toggleTempMode() {
  TEMP_MODE = (TEMP_MODE == 'c') ? 'f' : 'c';
  updateStatus();
}

updateStatus();
updateEnvironmentalTrends();
setInterval(updateStatus, 15000);
setInterval(updateEnvironmentalTrends, 150000);
