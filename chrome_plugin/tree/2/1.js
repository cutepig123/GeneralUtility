var data = {
    id: 0,
    title: "root",
    children: [{
        id: 1,
        title: "Option 1",
        children: [{
            id: 11,
            title: "Option 11",
            children: [{
                id: 111,
                title: "Option 111",
				isichecked: 0
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
	var item = branch;
	var myimg;
	var ul;
	var mychkbox;
	var myitem = $('<li>', {
		id: "item" + item.id
	});
	if (item.children){
		myimg =$('<img>', {
			src: "m.gif",
		});
		myitem.append(myimg);
	};
	mychkbox =$('<input>', {
		type: "checkbox",
		id: "item" + item.id,
		name: "item" + item.id
	});
	
	myitem.append(mychkbox);
	myitem.append($('<label>', {
		for: "item" + item.id,
		text: item.title
	}));
	parentUL.append(myitem);
	if (item.children) {
		ul = $('<ul>', {
			style: 'display: show'
		}).appendTo(myitem);
		myitem.append();
		
		for (var key in branch.children) {
			var itemc = branch.children[key];
			addItem(ul, itemc);
		}
	}
	
	if(myimg){
		myimg.click(function () {
			if(myimg.attr('src')==="m.gif"){	// show->hide
				myimg.attr('src',"p.gif");
				ul.hide();
			}else{			// hide->show
				myimg.attr('src',"m.gif");
				ul.show();
			}	
		});
	}
	
	mychkbox.change(function () {
		var isichecked =mychkbox.prop('checked');
		item.isichecked =isichecked;
	   if(ul){
			var childchks=ul.find(':checkbox');
			childchks.prop('checked',isichecked);
	   }
    });
	
	var isichecked=true;
	if(typeof item.isichecked !== typeof undefined){
		isichecked =item.isichecked;
	}
	mychkbox.prop('checked',isichecked);
}

$(function () {
    addItem($('#root'), data);
	// $('img').click(function () {
        // if($(this).attr('src')==="m.gif"){	// show->hide
			// $(this).attr('src',"p.gif");
			// $(this).closest('li').children('ul').hide();
		// }else{			// hide->show
			// $(this).attr('src',"m.gif");
			// $(this).closest('li').children('ul').show();
		// }	
    // });
    // $(':checkbox').change(function () {
       // $(this).closest('li').children('ul').children(':checkbox').prop('checked',1);
    // });
});