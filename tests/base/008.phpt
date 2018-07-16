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
$a = sentry_enable_debug(true);
var_dump($a);
?>
--EXPECT--
The extension sentry is loaded and working!
NULL
