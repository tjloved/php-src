--TEST--
Generator with return type does not fail with empty return

--FILE--
<?php

function fn388374575()
{
    $a = function () : \Iterator {
        (yield 1);
        return;
    };
    foreach ($a() as $value) {
        echo $value;
    }
}
fn388374575();
--EXPECT--
1
