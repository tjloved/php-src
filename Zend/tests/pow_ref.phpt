--TEST--
Use power operator on reference
--FILE--
<?php

function fn362513671()
{
    $a = 2;
    $b = 3;
    $ref =& $b;
    $a **= $b;
    var_dump($a);
}
fn362513671();
--EXPECT--
int(8)
