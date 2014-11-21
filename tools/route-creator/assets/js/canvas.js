
var CANVAS = new Module();

CANVAS.bind('ready', function() {
  CANVAS.context = $('#canvas').get(0).getContext('2d');
});

CANVAS.bind('resize', function(size) {
  CANVAS.context.canvas.width  = size[0];
  CANVAS.context.canvas.height = size[1];
});

CANVAS.bind('tick', function() {
  var cc = CANVAS.context;

  cc.clearRect(0, 0, cc.canvas.width, cc.canvas.height);

  cc.save();

  cc.translate(cc.canvas.width * 0.5, cc.canvas.height * 0.5);

  var pos = UI.fire('convert', [0, 0]);
  cc.translate(pos[0] - 50, pos[1] - 50);

  cc.fillRect(0, 0, 100, 100);

  cc.restore();
});
