/*
 * @file
 * JavaScript for ajax_example.
 *
 * See @link ajax_example_dependent_dropdown_degrades @endlink for
 * details on what this file does. It is not used in any other example.
 */

(function($) {

  // Re-enable form elements that are disabled for non-ajax situations.
  Drupal.behaviors.banner = {
    attach: function() {
      if($('.banner-default').length) {
        $('.banner-default').popover({ delay: { hide:1000, show: 100 }, trigger:'hover', placement:'top', html:1, content:"You can <a href='/admin/structure/types/manage/banner/fields/field_banner' target='_blank'>click here</a> to set the default image"});
      }
      
      if($('select#edit-bannernodes').length) {
        $('select#edit-bannernodes').multiselect().multiselectfilter();
      }
      /**
       * Choose a banner
       */
      if( $('.form-item-banner input:checked').attr('value') !== 0 ) {
        var image = $('.form-item-banner input:checked').next('img').clone();
        $('.chosen-banner').html(image);
      }

      $('.form-item-banner input').live('change',
        function(){
          var image = $(this).next('img').clone();
          $('.chosen-banner').html(image);
        }
      );
    }
  };

})(jQuery);
