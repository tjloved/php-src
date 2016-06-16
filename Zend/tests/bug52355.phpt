--TEST--
Bug #52355 (Negating zero does not produce negative zero)
--FILE--
<?php

function fn67397745()
{
    var_dump(-0.0);
    var_dump(-(double) "0");
    $foo = -sin(0);
    var_dump($foo);
    var_dump(@(1.0 / -0.0));
}
fn67397745();
--EXPECT--
float(-0)
float(-0)
float(-0)
float(-INF)
