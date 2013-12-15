(function($) {
	$.fn.mtAnnounce = function(options) {

		var options = $.extend({
			className : 'notice',
			text : null}, options);
	
		return this.each(function() {
			$(this).prepend('<div class="' + options.className + '" />');
			
			var $announceDiv = $('.' + options.className);
            
             $announceDiv.append('<div>' + options.text + '</div>')
                         .prepend('<span class="close">X</span>')
                         .children('div').append('<span class="closein">关闭</span>');
              
			function screen(){
                $announceDiv
                    .css({
                    'position':'relative',
                    'width':$(window).width(),
                    'height':$(window).height()
                    })
                    
                    .children('div')
					.css({
                        'position':'absolute',
                        'display':'block',
                        'left':'50%',
                        'top':'50%'
					})
                    .end()
                    .children('.close')
					.css({
						'position' : 'absolute',
						'cursor' : 'pointer',
						'display' : 'none'
					})
                    .end()

                    .hover(function() {
					// over
                        $(this)
                            .children('.close')
                            .show()
                            .click(function(){close()});

                        }, function() {
                            $('.close').hide();
                    });

              $announceDiv.children('div').css({
                   'marginLeft':-$announceDiv.children('div').width()/2,
                   'marginTop':-$announceDiv.children('div').height()/2
              })

               }

            $('.closein').click(function(){close()});

            screen()
            
            function close() {
                $announceDiv.slideUp(250, function() {
                    $(this).remove();
                });
            }

            $(window).resize(function(){
                screen()
            })

		}); // end each
	}
	
})(jQuery);