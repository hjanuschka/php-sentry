--TEST--
Test Static SentryNative class
--SKIPIF--
<?php
if (!extension_loaded('sentry')) {
	echo 'skip';
}
?>
--FILE--
<?php
use \Sentry\Client\Native as SentryNative;
$a = SentryNative::test();
var_dump($a);
?>
--EXPECTF--
string(11) "Hello World"
