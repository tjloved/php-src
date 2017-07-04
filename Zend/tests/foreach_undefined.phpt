--TEST--
foreach() & undefined var
--FILE--
<?php

function fn147440012()
{
    foreach ($a as $val) {
    }
    echo "Done\n";
}
fn147440012();
--EXPECTF--	
Notice: Undefined variable: a in %s on line %d

Warning: Invalid argument supplied for foreach() in %s on line %d
Done
