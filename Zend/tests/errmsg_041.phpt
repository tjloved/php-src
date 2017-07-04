--TEST--
errmsg: instanceof expects an object instance, constant given
--FILE--
<?php

function fn1315854788()
{
    var_dump("abc" instanceof stdclass);
    echo "Done\n";
}
fn1315854788();
--EXPECTF--	
Fatal error: instanceof expects an object instance, constant given in %s on line %d
