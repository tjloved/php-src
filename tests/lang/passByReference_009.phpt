--TEST--
Assignement as argument
--FILE--
<?php

function foo(&$x, &$y)
{
    $x = 1;
    echo $y;
}
// prints 1 ..
function foo2($x, &$y, $z)
{
    echo $x;
    // 0
    echo $y;
    // 1
    $y = 2;
}
function fn1440143602()
{
    $x = 0;
    foo($x, $x);
    $x = 0;
    foo2($x, $x, $x = 1);
    echo $x;
    // 2
}
fn1440143602();
--EXPECTF--
1012
