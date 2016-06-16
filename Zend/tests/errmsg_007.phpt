--TEST--
errmsg: can't use [] for reading - 2
--FILE--
<?php

function fn1011738977()
{
    $a = array();
    isset($a[]);
    echo "Done\n";
}
fn1011738977();
--EXPECTF--	
Fatal error: Cannot use [] for reading in %s on line %d
