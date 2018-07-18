--TEST--
undefined function name, no debug output
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
ini_set("display_errors", "on");
use \Sentry\Client\Native as SentryClient;

SentryClient::addBreadCrumb("Message", "manual");

class S1 {
    public function test() {
        echo HHHH;
        echo 1/0;
        var_dump(SentryClient::last_error_id);
        throw new \Exception("exx");
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
