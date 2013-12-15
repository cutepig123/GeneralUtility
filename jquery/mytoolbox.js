(function($) {
    $.fn.mytoolbox = function(settings) {
		var _defaultSettings = {
			callback: function () {
				alert(this.id);
			}
		};
		var _settings = $.extend(_defaultSettings, settings);
		return this.each(function() {
			$(this).click(_settings.callback);
		});
	};
})(jQuery);