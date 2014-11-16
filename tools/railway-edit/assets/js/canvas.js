
var CANVAS = new Module();

CANVAS.bind('ready', function() {
  CANVAS.context = $('#canvas').get(0).getContext('2d');
});

CANVAS.bind('resize', function(size) {
  CANVAS.context.canvas.width  = size[0];
  CANVAS.context.canvas.height = size[1];
});

CANVAS.bind('tick', function() {
  CANVAS.context.fillRect(0, 0, 1000, 100);
});
