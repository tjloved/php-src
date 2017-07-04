--TEST--
Bug #70804 (Unary add on negative zero produces positive zero)
--FILE--
<?php

function fn677169036()
{
    var_dump(+-0.0);
    var_dump(+(double) "-0");
    $foo = +-sin(0);
    var_dump($foo);
}
fn677169036();
--EXPECT--
float(-0)
float(-0)
float(-0)
