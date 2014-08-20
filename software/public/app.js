/*globals $, dewPoint */

'use strict';

var TEMP_MODE = localStorage.getItem('tempMode') || 'f';

function formatTemperature(degreeC) {
  if (!TEMP_MODE || TEMP_MODE === 'c') {
    return Math.round(degreeC) + '°C';
  } else {
    return Math.round(degreeC * 1.8000 + 32.00)  + '°F';
  }
}

function renderStatus(status) {
  $(document.body).toggleClass('no-fan-speed', status.mode === 'DRY').
    toggleClass('no-temp-target', status.mode === 'FAN').
    toggleClass('power-off', !status.running).
    toggleClass('power-on', status.running);

  $('.power').toggleClass('on', status.running).toggleClass('off', !status.running);
  $("[data-cmd='mode/next'] .desc").text('Mode: ' + status.mode.toLowerCase().replace(/_/, ' '));
  $("[data-cmd='fan-speed/next'] .desc").text('Fan: ' + status.fanSpeed.toLowerCase().replace(/_/, ' '));
  $("#target-temp").text(status.temp);
}

function renderEnvironment(environment) {
  $("#env-temperature").text(formatTemperature(environment.temperature));
  $("#env-humidity").text(environment.humidity + '%');
  $("#env-dewpoint").text(formatTemperature(environment.dewPoint));
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

$('.core-metrics').click(function () {
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
      nowHumidity = data.log.humidity[count - 1],
      thenHumidity = data.log.humidity[count - lookBack],
      nowTemperature = data.log.temperature[count - 1],
      thenTemperature = data.log.temperature[count - lookBack],
      $humidity = $('.number.humidity'),
      $temperature = $('.number.temperature');

    console.log('humidity was', thenHumidity, 'is now', nowHumidity);
    console.log('temperature was', thenTemperature, 'is now', nowTemperature);

    if (nowHumidity == thenHumidity) {
      $humidity.removeClass('rising', 'falling');
    } else if (nowHumidity < thenHumidity) {
      $humidity.removeClass('rising').addClass('falling');
    } else if (nowHumidity > thenHumidity) {
      $humidity.removeClass('falling').addClass('rising');
    }

    if (nowTemperature == thenTemperature) {
      $temperature.removeClass('rising', 'falling');
    } else if (nowTemperature < thenTemperature) {
      $temperature.removeClass('rising').addClass('falling');
    } else if (nowTemperature > thenTemperature) {
      $temperature.removeClass('falling').addClass('rising');
    }
  }, true);
}

function sendResetCommand() {
  if(confirm("Reset AC state to: cool, 70F, fan speed low, off?")) {
    runCmd('reset');
  }
}

function toggleTempMode() {
  TEMP_MODE = (TEMP_MODE == 'c') ? 'f' : 'c';
  updateStatus();
}

updateStatus();
updateEnvironmentalTrends();
setInterval(updateStatus, 15000);
setInterval(updateEnvironmentalTrends, 150000);
