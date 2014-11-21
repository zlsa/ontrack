
var UI = new Module();

UI.Prompt = Class.extend({
  init: function(options) {
    if(!options) options = {};

    this.title = '';
    this.desc  = '';
    this.buttons = [
      ['OK',     true]
    ];

    this.callback = null;

    this.create();

    this.set(options);
  },

  set: function(data) {
    if('title' in data) {
      this.title = data.title;
    }

    if('desc' in data) {
      this.desc  = data.desc;
    }

    if('buttons' in data) {
      this.buttons = data.buttons;
    }

    if('callback' in data) {
      this.callback = data.callback;
    }

    this.update();
  },

  create: function() {
    this.html = $('<div></div>');
    this.html.addClass('prompt');
    this.html.append('<header></header>');

    var header = this.html.find('header');
    header.append('<h1></h1>');
    header.append('<a class="close-button"></a>');

    var scope = this;
    header.find('.close-button').click(function() {
      scope.hide.call(scope);
    });

    this.html.append('<main></main>');
    this.html.append('<div class="button-row"></div>');

    this.html.addClass('hidden');

    $('#prompts').append(this.html);
  },

  update: function() {
    this.html.find('header h1').text(this.title);
    this.html.find('main').text(this.desc);
    this.html.find('.button-row').empty();

    if(this.desc == '') this.html.removeClass('title');
    else                this.html.addClass('title');

    var scope = this;

    for(var i=0;i<this.buttons.length;i++) {
      this.html.find('.button-row').append('<span class="button button-' + i + '"></span>');
      this.html.find('.button-row .button-' + i).text(this.buttons[i][0]);
      this.html.find('.button-row .button-' + i).data('index', i);
      this.html.find('.button-row .button-' + i).click(function() {
        var index = $(this).data('index');
        scope.button.call(scope, index);
      });
    }

  },

  button: function(index) {
    if(this.callback)
      this.callback(this.buttons[index][1]);
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

UI.bind('ready', function() {
  UI.veil = 0;

  $('body').append('<div id="prompts"></div>');
  $('body').append('<div id="veil"></div>');
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

UI.bind('prompt', function(options) {
  new UI.Prompt(options).show();
});
