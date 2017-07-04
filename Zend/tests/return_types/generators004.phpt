--TEST--
Generator with return type does not fail with empty return

--FILE--
<?php

function fn2038649105()
{
    $a = function () : \Iterator {
        (yield 1);
        return;
    };
    foreach ($a() as $value) {
        echo $value;
    }
}
fn2038649105();
--EXPECT--
1
