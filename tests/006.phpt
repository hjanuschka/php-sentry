--TEST--
sentry_test1() Basic test
--SKIPIF--
<?php
if (!extension_loaded('sentry')) {
	echo 'skip';
}
?>
--FILE--
<?php
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
--EXPECT--
The extension sentry is loaded and working!
NULL
