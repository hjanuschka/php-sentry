--TEST--
parse error
--SKIPIF--
<?php
if (!extension_loaded('sentry')) {
	echo 'skip';
}
?>
--FILE--
<?php
sentry_send_sample();
echo "1";
?>
--EXPECTF--
1
