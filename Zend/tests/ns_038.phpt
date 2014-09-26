--TEST--
038: Name ambiguity (namespace name or internal class name)
--FILE--
<?php
namespace Exception;
function foo() {
  echo "ok\n";
}
\Exception\foo();
\Exception::bar();
--EXPECTF--
ok

Fatal error: Uncaught exception 'EngineException' with message 'Call to undefined method Exception::bar()' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
