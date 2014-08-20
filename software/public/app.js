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
  $("#env-temp").text(formatTemperature(environment.temp));
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

function runCmd(cmd) {
  setLoadingCount(1);
  $.getJSON('/cmd/' + cmd).then(function (d) {
    setLoadingCount(-1);
    renderStatus(d.result);
  }, function () {
    setLoadingCount(-1);
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
  $.getJSON('/cmd/status', function (statusResp) {
    renderStatus(statusResp.result);
    setTimeout(function () {
      $.getJSON('/cmd/environment', function (envResp) {
        if (envResp.result.environment.temp) {
          envResp.result.environment.dewPoint = dewPoint(envResp.result.environment.humidity, envResp.result.environment.temp);
        }
        renderEnvironment(envResp.result.environment);
      });
    }, 250);
  });
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
// setInterval(updateStatus, 15000);
