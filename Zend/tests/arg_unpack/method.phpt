--TEST--
Unpack arguments for method calls
--FILE--
<?php

class Foo
{
    public function test(...$args)
    {
        var_dump($args);
    }
    public static function test2(...$args)
    {
        var_dump($args);
    }
}
function fn7161335()
{
    $foo = new Foo();
    Foo::test2(1, 2, ...[3, 4], ...[], ...[5]);
}
fn7161335();
--EXPECT--
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
}
