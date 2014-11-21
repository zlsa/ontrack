
var Point = Class.extend({
  init: function(pos, slave) {
    this.pos = pos || [0, 0];

    this.slave = slave || false;
  }
});

var Bezier = Class.extend({
  init: function() {
    this.points = [];
  },

  push: function(point) {
    this.points.push(point);
  }
});
