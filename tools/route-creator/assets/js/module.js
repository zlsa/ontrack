
var Module = Class.extend({
  init: function() {
    this.events = {};
  },
  bind: function(event, callback, data) {
    if(!(event in this.events)) {
      this.events[event] = [];
    }
    this.events[event].push({
      scope:    this,
      callback: callback,
      data:     data
    });
  },
  fire: function(event, data) {

    if(!(event in this.events)) {
      return null;
    }

    var ret = null;
    for(var i=0; i<this.events[event].length; i++) {
      var e = this.events[event][i];
      ret = e.callback.call(e.scope, data, e.data);
    }

    return ret;
  }
});
