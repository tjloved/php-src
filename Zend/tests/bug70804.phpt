--TEST--
Bug #70804 (Unary add on negative zero produces positive zero)
--FILE--
<?php

function fn660905712()
{
    var_dump(+-0.0);
    var_dump(+(double) "-0");
    $foo = +-sin(0);
    var_dump($foo);
}
fn660905712();
--EXPECT--
float(-0)
float(-0)
float(-0)
