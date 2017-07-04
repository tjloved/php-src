--TEST--
Unpack arguments for new expression
--FILE--
<?php

class Foo
{
    public function __construct(...$args)
    {
        var_dump($args);
    }
}
function fn1058473421()
{
    new Foo(...[]);
    new Foo(...[1, 2, 3]);
    new Foo(...[1], ...[], ...[2, 3]);
}
fn1058473421();
--EXPECT--
array(0) {
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
