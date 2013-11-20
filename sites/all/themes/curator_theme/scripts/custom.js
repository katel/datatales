// custom js for curatorial

(function ($) {
  Drupal.behaviors.gridModule = {
    attach: function (context, settings) {
      $('.gridly').gridly({
    base: 60, // px
    gutter: 20, // px
    columns: 12
  });
    }
  };
})(jQuery);


jQuery(function(){ //DOM Ready

    jQuery(".view-drag-and-drop-demo .views-row").gridster({
        widget_margins: [10, 10],
        widget_base_dimensions: [140, 140]
    });

});


(function ($) {
  Drupal.behaviors.gridModule = {
    attach: function (context, settings) {
$( ".sortable" ).sortable();
$( ".sortable" ).disableSelection();


    }
  };
})(jQuery);
