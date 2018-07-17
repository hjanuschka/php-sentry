--TEST--
Debugg output if ini is set
--SKIPIF--
<?php
if (!extension_loaded('sentry')) {
	echo 'skip';
}
?>
--INI--
sentry.debug=true
--FILE--
<?php
echo HHH;
?>
--EXPECTF--
SENTRY PHP-EXT Catched:
==============
message: Use of undefined constant HHH - assumed 'HHH'
	Frame(0):
		file: %s
		lineo: 2
		type: 8
/SENTRY PHP-EXT Catched:

Notice: Use of undefined constant HHH - assumed 'HHH' in %s on line 2
HHH
