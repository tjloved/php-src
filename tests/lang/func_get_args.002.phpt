--TEST--
func_get_args with variable number of args
--FILE--
<?php

function foo($a)
{
    var_dump(func_get_args());
}
function fn1752295229()
{
    foo(1, 2, 3);
}
fn1752295229();
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}

