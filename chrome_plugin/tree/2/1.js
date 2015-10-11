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
        $item = $('<li>', {
            id: "item" + item.id
        });
        $item.append($('<input>', {
            type: "checkbox",
            id: "item" + item.id,
            name: "item" + item.id
        }));
        $item.append($('<label>', {
            for: "item" + item.id,
            text: item.title
        }));
        parentUL.append($item);
        if (item.children) {
            var $ul = $('<ul>', {
                style: 'display: none'
            }).appendTo($item);
            $item.append();
            addItem($ul, item);
        }
    }
}

$(function () {
    addItem($('#root'), data);
    $(':checkbox').change(function () {
        $(this).closest('li').children('ul').toggle();
    });
    $('label').click(function(){
        $(this).closest('li').find(':checkbox').trigger('click');
    });
});