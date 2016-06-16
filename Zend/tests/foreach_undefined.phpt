--TEST--
foreach() & undefined var
--FILE--
<?php

function fn1448310608()
{
    foreach ($a as $val) {
    }
    echo "Done\n";
}
fn1448310608();
--EXPECTF--	
Notice: Undefined variable: a in %s on line %d

Warning: Invalid argument supplied for foreach() in %s on line %d
Done
