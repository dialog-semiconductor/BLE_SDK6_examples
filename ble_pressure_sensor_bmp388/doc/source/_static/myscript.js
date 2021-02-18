/* $( document ).ready(function() {
	$("span.dk_popup_explanation").mouseover(function() {
		$(this).animate({ backgroundColor: "black" }, 1000);
		$(this).children(".dk_popup_description").show();
	}).mouseout(function() {
		$(this).animate({ backgroundColor: "#68BFEF" }, 500);
		$(this).children(".dk_popup_description").hide();
	});
});
*/

$(function() {
    // $( document ).tooltip();
	// $('span.has_popup_explanation').tooltip();
	$( 'span.has_popup_explanation' ).tooltip({
	      position: {
	        my: "center bottom-20",
	        at: "center top",
	        using: function( position, feedback ) {
	          $( this ).css( position );
	          $( "<div>" )
	            .addClass( "arrow" )
	            .addClass( feedback.vertical )
	            .addClass( feedback.horizontal )
	            .appendTo( this );
	        }
	      }
    });
});

$(function() {

    var $sidebar   = $(".sphinxsidebar"), 
        $window    = $(window),
        offset     = $sidebar.offset(),
        topPadding = 15;

    $window.scroll(function() {
        if ($window.scrollTop() > offset.top) {
            $sidebar.stop().animate({
                marginTop: $window.scrollTop() - offset.top + topPadding
            });
        } else {
            $sidebar.stop().animate({
                marginTop: 0
            });
        }
    });

});
