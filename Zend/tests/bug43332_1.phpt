--TEST--
Bug #43332.1 (self and parent as type hint in namespace)
--FILE--
<?php
namespace foobar;

class foo {
  public function bar(self $a) { }
}

$foo = new foo;
$foo->bar($foo); // Ok!
$foo->bar(new \stdclass); // Error, ok!
--EXPECTF--
Fatal error: Uncaught exception 'EngineException' with message 'Argument 1 passed to foobar\foo::bar() must be an instance of foobar\foo, instance of stdClass given, called in %s on line %d and defined' in %s:%d
Stack trace:
#0 %s(%d): foobar\foo->bar(Object(stdClass))
#1 {main}
  thrown in %s on line %d
