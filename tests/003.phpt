--TEST--
sentry_test2() Basic test
--SKIPIF--
<?php
if (!extension_loaded('sentry')) {
	echo 'skip';
}
?>
--FILE--
<?php
ini_set('sentry.debug', true);
echo HHH;
?>
--EXPECT--
string(11) "Hello World"
string(9) "Hello PHP"
