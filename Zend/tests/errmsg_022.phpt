--TEST--
errmsg: only variables can be passed by reference
--FILE--
<?php

function foo(&$var)
{
}
function fn1257645075()
{
    foo(1);
    echo "Done\n";
}
fn1257645075();
--EXPECTF--	
Fatal error: Only variables can be passed by reference in %s on line %d
