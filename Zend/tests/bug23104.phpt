--TEST--
Bug #23104 (Hash position not reset for constant arrays)
--FILE--
<?php

function foo($bar = array("a", "b", "c"))
{
    var_dump(current($bar));
}
function fn1191931058()
{
    foo();
}
fn1191931058();
--EXPECT--
string(1) "a"
