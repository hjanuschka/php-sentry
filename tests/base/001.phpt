--TEST--
Check if sentry is loaded
--SKIPIF--
<?php
if (!extension_loaded('sentry')) {
	echo 'skip';
}
?>
--FILE--
<?php 
echo 'The extension "sentry" is available';
?>
--EXPECT--
The extension "sentry" is available
