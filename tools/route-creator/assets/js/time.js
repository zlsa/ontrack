
var Time = Class.extend({
  init: function(options) {
    if(!options) options = {};

    this.time   = new Date().getTime() * 0.001;
    this.delta  = 0;
    this.factor = 1;

    if('time' in options)
      this.time = options.time;

  },
  set: function(time) {
    if(!time) time = new Date().getTime() * 0.001;
    this.add(time - this.time);
  },
  add: function(delta) {
    delta      *= this.factor;
    this.time  += delta;
    this.delta  = delta;
  }
});

var TIME = new Module();

TIME.start = new Time();

TIME.bind('ready', function() {
  TIME.ready = new Time();
  TIME.time  = new Time();

  console.log('startup: ' + Math.round((TIME.ready.time - TIME.start.time) * 1000) + 'ms');
});

TIME.bind('tick', function() {
  TIME.time.set();
  return TIME.time.delta;
});
