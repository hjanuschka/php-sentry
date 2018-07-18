--TEST--
Debugg output on userspace request
--SKIPIF--
<?php
if (!extension_loaded('sentry')) {
	echo 'skip';
}
?>
--FILE--
<?php
sentry_enable_debug(true);
error_reporting(0);
ini_set("display_errors", "off");
class S1 {
    public function test() {
        throw new \Exception("AAAA");
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
message: AAAA
	Frame(0):
		file: %s
		lineo: 7
		type: 1
	Frame(1):
		file: %s
		lineo: 16
		class: Sentry
		function: __construct
	Frame(2):
		file: %s
		lineo: 13
		class: S1
		function: test
/SENTRY PHP-EXT Catched:
