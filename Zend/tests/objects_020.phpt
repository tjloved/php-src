--TEST--
Accessing members of standard object through of variable variable
--FILE--
<?php

function fn1769984317()
{
    error_reporting(E_ALL);
    $test = 'stdclass';
    ${$test}->a =& ${$test};
    ${$test}->a->b[] = 2;
    var_dump(${$test});
}
fn1769984317();
--EXPECTF--
object(stdClass)#%d (2) {
  ["a"]=>
  *RECURSION*
  ["b"]=>
  array(1) {
    [0]=>
    int(2)
  }
}
