
var EVENT = new Module();

EVENT.bind('ready', function() {

  $(window).on('click', '[data-event]', function(e) {
    var el       = $(e.target);
    var event    = el.attr('data-event');
    var category = event.substr(0, event.indexOf('-'));
    var action   = event.substr(event.indexOf('-') + 1);
    var data     = el.attr('data-data');
    EVENT.fire('event', [category, action, data])
  });

  $(window).on('contextmenu', function(e) {
    return false;
  });

});

EVENT.bind('resize', function(size) {

});

EVENT.bind('event', function(ev) {
  var category = ev[0];
  var action   = ev[1];
  var data     = ev[2];

  console.log(ev);

  if(category == 'route') {
    ROUTE.fire('event', ev);
  } else if(category == 'help') {

    UI.fire('menu', {
      items: [
        ['text',   'To pan the map, drag with the middle mouse button.'],
        ['text',   'To add a new node between two nodes, drag the gray dot between them.'],
        ['text',   'To create a new track, right-click and select "add track".'],
        ['button', 'Close']
      ]
    });

  }

});
