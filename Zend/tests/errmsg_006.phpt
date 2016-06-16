--TEST--
errmsg: can't use [] for reading
--FILE--
<?php

function fn1750933313()
{
    $a = array();
    $b = $a[];
    echo "Done\n";
}
fn1750933313();
--EXPECTF--	
Fatal error: Cannot use [] for reading in %s on line %d
