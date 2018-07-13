--TEST--
sentry_test1() Basic test
--SKIPIF--
<?php
if (!extension_loaded('sentry')) {
	echo 'skip';
}
?>
--FILE--
<?php 
$ret = sentry_test1();

var_dump($ret);
?>
--EXPECT--
The extension sentry is loaded and working!
NULL
