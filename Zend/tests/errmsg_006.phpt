--TEST--
errmsg: can't use [] for reading
--FILE--
<?php

function fn242935560()
{
    $a = array();
    $b = $a[];
    echo "Done\n";
}
fn242935560();
--EXPECTF--	
Fatal error: Cannot use [] for reading in %s on line %d
