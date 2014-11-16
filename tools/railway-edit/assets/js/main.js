
$(document).ready(function() {
  TIME.fire('ready');
  CANVAS.fire('ready');
  EVENT.fire('ready');

  $(window).resize(resize);

  resize();
  tick();
});

function resize() {
  CANVAS.fire('resize', [$(window).width(), $(window).height()]);
}

function tick() {
  var delta = TIME.fire('tick');

  CANVAS.fire('tick', delta);

  requestAnimationFrame(tick);
}
