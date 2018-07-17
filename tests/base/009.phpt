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
Parse error: syntax error, unexpected ';', expecting variable (T_VARIABLE) or '{' or '$' in %s on line 2
