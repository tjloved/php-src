--TEST--
Bug #32833 (Invalid opcode with $a[] .= '')
--FILE--
<?php

function fn1140658100()
{
    $test = array();
    $test[] .= "ok\n";
    echo $test[0];
}
fn1140658100();
--EXPECT--
ok
