--TEST--
Ensure "undefined offset" notice formats message corectly when undefined key is negative
--FILE--
<?php

function fn1661171061()
{
    [][-1];
    [][-1.1];
    (new ArrayObject())[-1];
    (new ArrayObject())[-1.1];
    echo "Done\n";
}
fn1661171061();
--EXPECTF--	
Notice: Undefined offset: -1 in %s on line %d

Notice: Undefined offset: -1 in %s on line %d

Notice: Undefined offset: -1 in %s on line %d

Notice: Undefined offset: -1 in %s on line %d
Done
