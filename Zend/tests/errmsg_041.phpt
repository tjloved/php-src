--TEST--
errmsg: instanceof expects an object instance, constant given
--FILE--
<?php

function fn445157423()
{
    var_dump("abc" instanceof stdclass);
    echo "Done\n";
}
fn445157423();
--EXPECTF--	
Fatal error: instanceof expects an object instance, constant given in %s on line %d
