--TEST--
Method returned callable, expected callable

--FILE--
<?php

class foo
{
    public function bar() : callable
    {
        $test = "one";
        return function () use($test) : array {
            return array($test);
        };
    }
}
function fn530537381()
{
    $baz = new foo();
    var_dump($baz->bar());
}
fn530537381();
--EXPECT--
object(Closure)#2 (2) {
  ["static"]=>
  array(1) {
    ["test"]=>
    string(3) "one"
  }
  ["this"]=>
  object(foo)#1 (0) {
  }
}
