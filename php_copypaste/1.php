<?php
if( isset( $_FILES['file'] ) ) {
    $file_contents = file_get_contents( $_FILES['file']['tmp_name'] );
    header("Content-Type: " . $_FILES['file']['type'] );
    die($file_contents);
}
else {
    header("HTTP/1.1 400 Bad Request");
}
print_r($_FILES);
?>

<script>
document.onpaste = function (e) {
    var items = e.clipboardData.items;
    var files = [];
    for( var i = 0, len = items.length; i < len; ++i ) {
        var item = items[i];
        if( item.kind === "file" ) {
            submitFileForm(item.getAsFile(), "paste");
        }
    }

};

function submitFileForm(file, type) {
    var extension = file.type.match(/\/([a-z0-9]+)/i)[1].toLowerCase();
    var formData = new FormData();
    formData.append('file', file, "image_file");
    formData.append('extension', extension );
    formData.append("mimetype", file.type );
    formData.append('submission-type', type);

    var xhr = new XMLHttpRequest();
    xhr.responseType = "blob";
    xhr.open('POST', '<?php echo basename(__FILE__); ?>');
    xhr.onload = function () {
        if (xhr.status == 200) {
            var img = new Image();
            img.src = (window.URL || window.webkitURL)
                .createObjectURL( xhr.response );
            document.body.appendChild(img);
        }
    };

    xhr.send(formData);
}
</script>

Please click print screen and then click "Ctrl+V" here!
