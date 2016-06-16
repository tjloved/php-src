--TEST--
Use power operator on reference
--FILE--
<?php

function fn341633467()
{
    $a = 2;
    $b = 3;
    $ref =& $b;
    $a **= $b;
    var_dump($a);
}
fn341633467();
--EXPECT--
int(8)
