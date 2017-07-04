--TEST--
errmsg: only variables can be passed by reference
--FILE--
<?php

function foo(&$var)
{
}
function fn1289974334()
{
    foo(1);
    echo "Done\n";
}
fn1289974334();
--EXPECTF--	
Fatal error: Only variables can be passed by reference in %s on line %d
