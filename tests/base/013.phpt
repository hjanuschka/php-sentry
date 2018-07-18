--TEST--
chained exception
--SKIPIF--
<?php
if (!extension_loaded('sentry')) {
	echo 'skip';
}
?>
--INI--
sentry.debug=1
--FILE--
<?php
try
{
    throw new Exception('Exception 1', 1001);
}
catch (Exception $ex)
{
    throw new Exception('Exception 2', 1002, $ex);
}
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined function err1or_reporting() in %s:2
Stack trace:
#0 {main}
  thrown in %s on line 2
