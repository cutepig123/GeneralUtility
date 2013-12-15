;(function($) {
    $.fn.mytab = function(settings) {
		var _defaultSettings = {
			activeClass: 'active'
		};
		var _settings = $.extend(_defaultSettings, settings);
		var _handler = function() {
			var container = this;
			var $tabBlocks = $('div.tabBlock', container);
			var $tabLists = $('div.tabs li', container);
			var $tabLinks = $('div.tabs li a', container);
			$tabBlocks.hide().eq(0).show();
			$tabLinks.click(function () {
				$tabLists.removeClass(_settings.activeClass);
				$(this).parent('li').toggleClass(_settings.activeClass);
				$tabBlocks.hide();
				var id = (String(this.href).match(/(#.+)$/))[1];
				$(id).show();
				return false;
			});
		};
		return this.each(_handler);
	};
})(jQuery);