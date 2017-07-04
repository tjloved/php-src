--TEST--
Bug #72215 (Wrong return value if var modified in finally)
--FILE--
<?php

function test()
{
    $a = 1;
    try {
        return $a;
    } finally {
        $a = 2;
    }
}
function fn71409002()
{
    var_dump(test());
}
fn71409002();
--EXPECT--
int(1)
