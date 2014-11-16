
var EVENT = new Module();

EVENT.bind('ready', function() {
  $(window).on('click', '[data-event]', function(e) {
    console.log($(e.target).attr('data-event'));
  });
});

EVENT.bind('resize', function(size) {

});

EVENT.bind('tick', function() {

});
