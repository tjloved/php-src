--TEST--
Bug #60099 (__halt_compiler() works in braced namespaces)
--FILE--
<?php
namespace foo {
	__halt_compiler();

?>
--EXPECTF--
Fatal error: Uncaught exception 'ParseException' with message 'syntax error, unexpected end of file' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
