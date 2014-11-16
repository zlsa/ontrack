
var Root = Class.extend({
  init: function() {
    this.segments = [];
  }
});

var RAILWAY = new Module();

RAILWAY.bind('ready', function() {
  RAILWAY.roots = [];
});

RAILWAY.bind('open-prompt', function(ev) {

  alert('file opening is not implemented yet');

});


RAILWAY.bind('event', function(ev) {
  var category = ev[0];
  var action   = ev[1];
  var data     = ev[2];

  if(action == 'open') {
    RAILWAY.fire('open-prompt');
  }

});
