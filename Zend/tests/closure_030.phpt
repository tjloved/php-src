--TEST--
Closure 030: Using lambda with variable variables
--FILE--
<?php

function fn157468284()
{
    $b = function () {
        return func_get_args();
    };
    $a = 'b';
    var_dump(${$a}(1));
    var_dump(${$a}->__invoke(2));
}
fn157468284();
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(2)
}
