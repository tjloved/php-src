--TEST--
Type spec use as double initialization
--FILE--
<?php

function test($i)
{
    $a = 1;
    while ($i--) {
        $a = $a + 1.0;
    }
    var_dump($a);
}
function fn1754258171()
{
    test(1);
}
fn1754258171();
--EXPECT--
float(2)
