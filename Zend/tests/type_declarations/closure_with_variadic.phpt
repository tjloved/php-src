--TEST--
Closure with variadic type declaration
--FILE--
<?php

function fn1825361513()
{
    $f = function (stdClass ...$a) {
        var_dump($a);
    };
    $f(new stdClass());
}
fn1825361513();
--EXPECT--
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
