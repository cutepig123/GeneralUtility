<?php
  
  function is_contains_href($href)
  {
	  $is_exist =false;
	  $handle = fopen('/date.txt', 'r');
	  if ($handle) {
		while (($buffer = fgets($handle, 4096)) !== false) {
			$url =strstr($buffer, ' ', true);
			if( strcmp($url, $href)==0 )
			{
				$is_exist= true;
				break;
			}
		}
		
		fclose($handle);
	  }

	return $is_exist;
  }
  
  function dump_all()
  {
	  $is_exist =false;
	  $handle = fopen('/date.txt', 'r');
	  if ($handle) {
		while (($buffer = fgets($handle, 4096)) !== false) {
			$href =strstr($buffer, ' ', true);
			$title =strstr($buffer, ' ');
			echo '<a href =' . urldecode($href) . '>' . urldecode($title) . '</a><br>';
		}
		
		fclose($handle);
	  }

	return $is_exist;
  }
  
  function insert($href, $title)
  {
	$handle = fopen('/date.txt', 'a');
	fprintf($handle,'%s %s' . PHP_EOL, $href, $title);
	fclose($handle);
  }
  
  if( isset($_GET['href']) && isset($_GET['title']) )
  {
	  $href =$_GET['href'];
	  $title = $_GET['title'];
	  //echo $href . $title;
	  $href_en =urlencode($href);
	  $title_en =urlencode($title);
	  
	  if(!is_contains_href($href_en))
		insert($href_en, $title_en);
	
	 echo "window.onload = function(){alert('(Add to pocket ok)');};";
  }
  else
  {
	dump_all();
  }
?>


