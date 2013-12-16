<?php
header('Content-Type: text/event-stream');
header('Cache-Control: no-cache');

echo "retry: 1000\n\n";
//while (1) {
    $time = date('r');
	echo "data: The server time is: {$time}\n\n";
	flush();
    //usleep(100000); //1000000 = 1 seconds
//}

?>
