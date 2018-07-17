--TEST--
Disabled by default
--SKIPIF--
<?php
if (!extension_loaded('sentry')) {
	echo 'skip';
}
?>
--FILE--
<?php
$ret = notfound();
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined function notfound() in %s:2
Stack trace:
#0 {main}
  thrown in %s on line 2
