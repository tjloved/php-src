--TEST--
ZE2 A class can only implement interfaces
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php
class base {
}
	
class derived implements base {
}
?>
--EXPECTF--
Fatal error: Uncaught exception 'EngineException' with message 'derived cannot implement base - it is not an interface' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
