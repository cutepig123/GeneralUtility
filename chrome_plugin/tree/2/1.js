var data = {
    id: 0,
    title: "root - not displayed",
    children: [{
        id: 1,
        title: "Option 1",
        children: [{
            id: 11,
            title: "Option 11",
            children: [{
                id: 111,
                title: "Option 111"
            }, {
                id: 112,
                title: "Option 112"
            }]
        }, {
            id: 12,
            title: "Option 12"
        }]
    }, {
        id: 2,
        title: "Option 2",
        children: [{
            id: 21,
            title: "Option 21"
        }, {
            id: 22,
            title: "Option 22"
        }]
    }, {
        id: 3,
        title: "Option 3",
        children: [{
            id: 0,
            title: "Option 31"
        }, {
            id: 0,
            title: "Option 32"
        }]
    }]
};

function addItem(parentUL, branch) {
    for (var key in branch.children) {
        var item = branch.children[key];
        var myitem = $('<li>', {
            id: "item" + item.id
        });
		if (item.children){
			myitem.append($('<img>', {
				src: "p.gif",
			}));
		};
		//<img align="absmiddle" src="images/b.gif" class="zk_tree_icon_collapse">
        myitem.append($('<input>', {
            type: "checkbox",
            id: "item" + item.id,
            name: "item" + item.id
        }));
        myitem.append($('<label>', {
            for: "item" + item.id,
            text: item.title
        }));
        parentUL.append(myitem);
        if (item.children) {
            var $ul = $('<ul>', {
                style: 'display: none'
            }).appendTo(myitem);
            myitem.append();
            addItem($ul, item);
        }
    }
}

$(function () {
    addItem($('#root'), data);
	$('img').click(function () {
        if($(this).attr('src')==="m.gif"){	// show->hide
			$(this).attr('src',"p.gif");
			$(this).closest('li').children('ul').hide();
		}else{			// hide->show
			$(this).attr('src',"m.gif");
			$(this).closest('li').children('ul').show();
		}	
    });
    $(':checkbox').change(function () {
       // $(this).closest('li').children('ul').toggle();
    });
});