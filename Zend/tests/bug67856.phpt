--TEST--
Bug #67856 (Leak when using array_reduce with by-ref function)
--FILE--
<?php

function fn1067106883()
{
    $array = [1, 2, 3];
    var_dump(array_reduce($array, function (&$a, &$b) {
        return $a + $b;
    }, 0));
}
fn1067106883();
--EXPECT--
int(6)
