--TEST--
send class stacktrace
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
error_reporting(0);
ini_set("display_errors", "off");
class S1 {
    public function test() {
        echo HHHH;
    }
}
class Sentry {
    public function __construct() {
        $a = new S1();
        $a->test();
    }
}
$a = new Sentry();
?>
--EXPECTF--
SENTRY PHP-EXT Catched:
==============
message: Use of undefined constant HHHH - assumed 'HHHH'
	Frame(0):
		file: %s
		lineo: 6
		type: 8
	Frame(1):
		file: %s
		lineo: 15
		class: Sentry
		function: __construct
	Frame(2):
		file: %s
		lineo: 12
		class: S1
		function: test
/SENTRY PHP-EXT Catched:
HHHH
