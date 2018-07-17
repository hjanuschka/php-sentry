--TEST--
undefined function name, no debug output
--SKIPIF--
<?php
if (!extension_loaded('sentry')) {
	echo 'skip';
}
?>
--FILE--
<?php
err1or_reporting(0);
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
Fatal error: Uncaught Error: Call to undefined function err1or_reporting() in %s:2
Stack trace:
#0 {main}
  thrown in %s on line 2
