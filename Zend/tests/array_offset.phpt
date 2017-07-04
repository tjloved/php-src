--TEST--
Ensure "undefined offset" notice formats message corectly when undefined key is negative
--FILE--
<?php

function fn1387216208()
{
    [][-1];
    [][-1.1];
    (new ArrayObject())[-1];
    (new ArrayObject())[-1.1];
    echo "Done\n";
}
fn1387216208();
--EXPECTF--	
Notice: Undefined offset: -1 in %s on line %d

Notice: Undefined offset: -1 in %s on line %d

Notice: Undefined offset: -1 in %s on line %d

Notice: Undefined offset: -1 in %s on line %d
Done
