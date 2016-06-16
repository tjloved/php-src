--TEST--
Simple While Loop Test
--FILE--
<?php

function fn1904744606()
{
    $a = 1;
    while ($a < 10) {
        echo $a;
        $a++;
    }
}
fn1904744606();
--EXPECT--
123456789
