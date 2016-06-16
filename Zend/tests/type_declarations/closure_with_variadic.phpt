--TEST--
Closure with variadic type declaration
--FILE--
<?php

function fn1618351228()
{
    $f = function (stdClass ...$a) {
        var_dump($a);
    };
    $f(new stdClass());
}
fn1618351228();
--EXPECT--
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
