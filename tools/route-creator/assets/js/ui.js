
var UI = new Module();

UI.Menu = Class.extend({
  init: function(options) {
    if(!options) options = {};

    this.items = [
      ['OK',     true]
    ];

    this.position = null;

    this.callback = null;

    this.create();

    this.set(options);
  },

  set: function(data) {

    if('items' in data) {
      this.items = data.items;
    }

    if('callback' in data) {
      this.callback = data.callback;
    }

    if('position' in data) {
      this.position = data.position;
    }

    this.update();
  },

  create: function() {
    this.html = $('<div></div>');
    this.html.addClass('menu');

    this.html.append('<main><ul class="items"></ul></main>');

    this.html.addClass('hidden');

    $('#menus').append(this.html);
  },

  update: function() {
    this.html.find('.items').empty();

    var scope = this;

    for(var i=0;i<this.items.length;i++) {
      this.html.find('.items').append('<li class="item item-' + i + '"></li>');
      this.html.find('.items .item-' + i).text(this.items[i][1]);
      this.html.find('.items .item-' + i).data('index', i);
      this.html.find('.items .item-' + i).addClass(this.items[i][0]);
      this.html.find('.items .item-' + i).click(function() {
        var index = $(this).data('index');
        scope.selected.call(scope, index);
      });
    }

    if(this.position)
      this.html.css({
        top:  this.position[1],
        left: this.position[0]
      });

  },

  selected: function(index) {
    var item = this.items[index];
    if(item[0] == 'text') {
      return;
    }
    if(this.callback) {
      if(item.length >= 3)
        this.callback(item[2]);
    }
    this.hide();
    this.destroy();
  },

  show: function() {
    this.html.removeClass('hidden');
    UI.fire('veil', true);
  },

  hide: function() {
    this.html.addClass('hidden');
    UI.fire('veil', false);
  },

  destroy: function() {
    this.html.remove();
    delete this;
  }

});

UI.bind('convert', function(map) {
  var canvas = [map[0], map[1]];

  canvas[0] /= UI.zoom;
  canvas[1] /= UI.zoom;

  canvas[0] -= UI.pan[0];
  canvas[1] -= UI.pan[1];

  return canvas;
});

UI.bind('ready', function() {
  UI.veil = 0;

  UI.menus     = [];

  UI.button    = 0;
  UI.start_pos = [0, 0];
  UI.start_pan = [0, 0];

  UI.pan  = [0, 0]; // in pixels
  UI.zoom = 1;      // meters per pixel

  $('body').append('<div id="menus"></div>');
  $('body').append('<div id="veil"></div>');

  $(window).keyup(function(e) {
    UI.fire('keyup', e);
  });

  $('#canvas').click(function(e) {
    UI.fire('click', e);
  });

  $('#canvas').mousedown(function(e) {
    UI.fire('mousedown', e);
  });

  $('#canvas').mouseup(function(e) {
    UI.fire('mouseup', e);
  });

  $('#canvas').mousemove(function(e) {
    UI.fire('mousemove', e);
  });

});

UI.bind('click', function(e) {
  ROUTE.fire('click', e);
});

UI.bind('keyup', function(e) {
  if(e.which == 27) {
    for(var i=0;i<UI.menus.length;i++) {
      if(UI.menus[i]) {
        UI.menus[i].hide();
        UI.menus[i].destroy();
      }
    }
    UI.veil = 0;
  }
});

UI.bind('mousedown', function(e) {
  UI.start_pos = [e.pageX, e.pageY];
  UI.start_pan = [UI.pan[0], UI.pan[1]];
  UI.button = e.which;

  if(UI.button == 3) {

    UI.fire('menu', {
      items: [
        ['button', 'Add track', 'add-track'],
        ['button', 'Cancel',    false]
      ],
      position: UI.start_pos
    });

  }

});

UI.bind('mouseup', function(e) {
  UI.button = 0;
});

UI.bind('mousemove', function(e) {
  if(UI.button != 2) return;
  var offset = [UI.start_pos[0] - e.pageX, UI.start_pos[1] - e.pageY];
  UI.pan[0] = UI.start_pan[0] + offset[0];
  UI.pan[1] = UI.start_pan[1] + offset[1];
});

UI.bind('veil', function(veil) {
  if(veil) {
    UI.veil += 1;
  } else {
    UI.veil -= 1;
  }

  if(UI.veil == 1) {
    $('body').addClass('veil');
  } else {
    $('body').removeClass('veil');
  }

});

UI.bind('menu', function(options) {
  var menu = new UI.Menu(options);
  UI.menus.push(menu);
  menu.show();
});
