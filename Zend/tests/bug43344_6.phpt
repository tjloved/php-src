--TEST--
Bug #43344.6 (Wrong error message for undefined namespace constant)
--FILE--
<?php
namespace Foo;
echo namespace\bar."\n";
?>
--EXPECTF--
Fatal error: Uncaught exception 'EngineException' with message 'Undefined constant 'Foo\bar'' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
