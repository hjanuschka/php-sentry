--TEST--
No Debug output by default
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
--EXPECT--
HHHH

