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
--EXPECT--
Fatal error: Uncaught Error: Call to undefined function notfound() in /Users/hjanuschka/php-sentry/tests/base/002.php:2
Stack trace:
#0 {main}
  thrown in /Users/hjanuschka/php-sentry/tests/base/002.php on line 2
