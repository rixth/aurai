function renderStatus(st) {
  $('.power').toggleClass('on', st.running).toggleClass('off', !st.running);
  $("[data-cmd='mode/next'] .desc").text('Mode: ' + st.mode.toLowerCase().replace(/_/, ' '));
  $("[data-cmd='fan-speed/next'] .desc").text('Fan: ' + st.fanSpeed.toLowerCase().replace(/_/, ' '));
  $("#target-temp").text(st.temp);
  if (st.environment.temp) {
    $("#env-temp").text(Math.round(st.environment.temp * 1.8000 + 32.00));
    $("#env-humidity").text(st.environment.humidity);
    $("#env-dewpoint").text(Math.round(dewPoint(st.environment.humidity, st.environment.temp) * 1.8000 + 32.00));
  }
}

$('[data-cmd]').click(function (evt) {
  $.getJSON('/cmd/' + $(evt.currentTarget).attr('data-cmd')).then(function (d) {
    renderStatus(d.status);
  });
});

function updateStatus() {
  $.getJSON('/cmd/environment', function () {
    $.getJSON('/cmd/status', function (d) {
      renderStatus(d.status);
    });
  })
}

updateStatus();
setInterval(updateStatus, 15000);