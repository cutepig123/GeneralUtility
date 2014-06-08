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
 document.onpaste = function(e){
    var items = e.clipboardData.items;
    console.log(JSON.stringify(items));
    if (e.clipboardData.items[0].kind === 'file') {
            // get the blob
            var imageFile = items[0].getAsFile();
            console.log(imageFile);
            var reader = new FileReader();
            reader.onload = function(event) {
                console.log(event.target.result); // data url!
                submitFileForm(event.target.result, 'paste');
            };
            reader.readAsBinaryString(imageFile);
        }
    };

function submitFileForm(file, type) {
    var formData = new FormData();
    var myBlob = new Blob([file], { "type" : "image/png"} );
    formData.append('file', myBlob, 'file.jpg');
    formData.append('submission-type', type);

    var xhr = new XMLHttpRequest();
    xhr.open('POST', '/task/file');
    xhr.onload = function () {
        if (xhr.status == 200) {
            console.log('all done: ');
        } else {
            console.log('Nope');
        }
    };

    xhr.send(formData);
}
</script>

Please click print screen and then click "Ctrl+V" here!
