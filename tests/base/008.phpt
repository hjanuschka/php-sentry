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
$;a = sentry_enable_debug(true);
var_dump($a);
?>
--EXPECTF--
Parse error: syntax error, unexpected ';', expecting variable (T_VARIABLE) or '{' or '$' in %s on line 2
