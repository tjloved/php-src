--TEST--
Bug #67856 (Leak when using array_reduce with by-ref function)
--FILE--
<?php

function fn1023605041()
{
    $array = [1, 2, 3];
    var_dump(array_reduce($array, function (&$a, &$b) {
        return $a + $b;
    }, 0));
}
fn1023605041();
--EXPECT--
int(6)
