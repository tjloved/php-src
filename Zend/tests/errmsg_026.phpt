--TEST--
errmsg: cannot redeclare class
--FILE--
<?php

class stdclass
{
}
function fn1517939416()
{
    echo "Done\n";
}
fn1517939416();
--EXPECTF--	
Fatal error: Cannot declare class stdclass, because the name is already in use in %s on line %d
