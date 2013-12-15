(function($) {
    $.fn.mytoolbox = function() {
        return this.each(function() {
            $(this).click(function () {
                alert(this.id);
            });
        });
    };
})(jQuery);