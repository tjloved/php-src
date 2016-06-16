--TEST--
Static Variable Expressions
--FILE--
<?php

const bar = 2, baz = bar + 1;
function foo()
{
    static $a = 1 + 1;
    static $b = [bar => 1 + 1, baz * 2 => 1 << 2];
    static $c = [1 => bar, 3 => baz];
    var_dump($a, $b, $c);
}
function fn1948116595()
{
    foo();
}
fn1948116595();
--EXPECT--
int(2)
array(2) {
  [2]=>
  int(2)
  [6]=>
  int(4)
}
array(2) {
  [1]=>
  int(2)
  [3]=>
  int(3)
}
