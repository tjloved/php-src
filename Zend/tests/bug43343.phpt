--TEST--
Bug #43343 (Variable class name)
--FILE--
<?php
namespace Foo;
class Bar { }
$foo = 'bar';
var_dump(new namespace::$foo);
?>
--EXPECTF--
Fatal error: Uncaught exception 'ParseException' with message 'syntax error, unexpected '::' (T_PAAMAYIM_NEKUDOTAYIM), expecting \\ (T_NS_SEPARATOR)' in %s:5
Stack trace:
#0 {main}
  thrown in %s on line 5
