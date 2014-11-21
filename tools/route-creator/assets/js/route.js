
var Segment = Class.extend({
  init: function() {
    this.type = null;
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

ROUTE.bind('ready', function() {
  ROUTE.roots = [];
});

ROUTE.bind('clear-prompt', function(ev) {
  UI.fire('prompt', {
    title:   'Clear current route?',
    buttons: [
      ['Cancel', false],
      ['OK',     true]
    ],
    callback: function(choice) {
      if(choice) {
        ROUTE.fire('clear');
      }
    }
  });
});

ROUTE.bind('save-prompt', function(ev) {
  alert('route saving is not implemented yet');
});

ROUTE.bind('open-prompt', function(ev) {
  alert('route opening is not implemented yet');
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
