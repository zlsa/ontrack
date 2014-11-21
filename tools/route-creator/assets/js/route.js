
var Segment = Class.extend({
  init: function() {
    this.type = 'bezier';
  }
});

var Track = Class.extend({
  init: function() {
    this.points   = [];
    this.segments = [];
  }
});

var ROUTE = new Module();

ROUTE.bind('ready', function() {
  ROUTE.track = new Track();
});

ROUTE.bind('convert', function(mouse) {
  var map = [mouse[0], mouse[1]];

  map[0] += UI.pan[0];
  map[1] += UI.pan[1];

  map[0] *= UI.zoom;
  map[1] *= UI.zoom;

  return map;
});

ROUTE.bind('clear-prompt', function(ev) {

  UI.fire('menu', {
    items: [
      ['text',   'Clear current route?'],
      ['button', 'Cancel', false],
      ['button', 'OK',     true]
    ],
    callback: function(choice) {
      console.log(choice);
      if(choice) {
        ROUTE.fire('clear');
      }
    }
  });

});

ROUTE.bind('save-prompt', function(ev) {

  UI.fire('menu', {
    items: [
      ['text',   'Route saving isn\'t implemented yet.'],
      ['button', 'Close']
    ]
  });

});

ROUTE.bind('open-prompt', function(ev) {

  UI.fire('menu', {
    items: [
      ['text',   'Route opening isn\'t implemented yet.'],
      ['button', 'Close']
    ]
  });

});

ROUTE.bind('click', function(e) {

  var pos = ROUTE.fire('convert', [e.pageX, e.pageY]);

});

ROUTE.bind('event', function(ev) {
  var category = ev[0];
  var action   = ev[1];
  var data     = ev[2];

  if(action == 'clear') {
    ROUTE.fire('clear-prompt');
  } else if(action == 'open') {
    ROUTE.fire('open-prompt');
  } else if(action == 'save') {
    ROUTE.fire('save-prompt');
  }

});
