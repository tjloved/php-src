--TEST--
Bug #32833 (Invalid opcode with $a[] .= '')
--FILE--
<?php

function fn1307020035()
{
    $test = array();
    $test[] .= "ok\n";
    echo $test[0];
}
fn1307020035();
--EXPECT--
ok
