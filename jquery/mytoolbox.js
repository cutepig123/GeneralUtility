(function($) {
    $.fn.mytoolbox = function(settings) {
		var _defaultSettings = {
			bind: 'click',
			callback: function () {
				alert(this.id);
			}
		};
		var _settings = $.extend(_defaultSettings, settings);
		return this.each(function() {
			$(this).bind(_settings.bind, _settings.callback);
		});
	};
})(jQuery);